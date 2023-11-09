#include "esphome/core/helpers.h"
#include "esphome/core/log.h"
#include "fourheat.h"

namespace esphome {
namespace fourheat {

static const char *const TAG = "fourheat";

static const uint16_t TX_THROTTLE = 10;
static const uint16_t RX_TIMEOUT = 100;
static const uint16_t RESPONSE_TIMEOUT = 300;
static const uint16_t RETRY_THROTTLE = 1000;
static const uint8_t MAX_RETRY_COUNT = 3;

static const uint8_t ID_LENGTH = 6;
static const uint8_t DATA_LENGTH = 12;
static const uint8_t MESSAGE_LENGTH = ID_LENGTH + DATA_LENGTH + 2;

void FourHeat::loop() {
  while(this->available()) {
    uint8_t c;
    this->read_byte(&c);
    this->handle_char_(c);
  }

  if (!this->rx_message_.empty() && millis() - this->last_rx_char_timestamp_ > RX_TIMEOUT) {
    ESP_LOGW(TAG, "Received incomplete data: %s", format_hex_pretty(this->rx_message_).c_str());
    this->rx_message_.clear();
  }

  process_next_tx_message_();
}

void FourHeat::update() {
  if (this->module_offline_) {
    return;
  }

  ESP_LOGV(TAG, "Updating...");

  if (this->last_sent_query_index_ >= this->queries_.size()) {
    this->last_sent_query_index_ = 0;
  }
}

void FourHeat::dump_config() {
  ESP_LOGCONFIG(TAG, "4Heat:");
}

void FourHeat::set_module_offline_sensor(binary_sensor::BinarySensor *module_offline_sensor) {
  this->module_offline_sensor_ = module_offline_sensor;
}

void FourHeat::register_query(const std::string &id) {
  std::vector<uint8_t> data;

  if (!create_message_(id, {}, data)) {
    return;
  }

  for (const auto &query : this->queries_) {
    if (query == data) {
      return;
    }
  }

  this->queries_.push_back(data);
}

void FourHeat::register_listener(const std::string &id, const std::function<void(const std::vector<uint8_t>)> &func) {
  this->listeners_.push_back(FourHeatDataListener{id, func});
}

void FourHeat::send_raw_value(const std::vector<uint8_t> &value) {
  ESP_LOGV(TAG, "Sending raw data: %s", format_hex_pretty(value).c_str());
  this->tx_queue_.push(value);
}

void FourHeat::send_bytes_value(const std::string &id, const std::vector<uint8_t> &value) {
  std::vector<uint8_t> data;

  if (!create_message_(id, value, data)) {
    return;
  }

  ESP_LOGV(TAG, "Queueing data: %s", format_hex_pretty(data).c_str());
  this->tx_queue_.push(data);
}

void FourHeat::send_string_value(const std::string &id, const std::string &value) {
  std::vector<uint8_t> data(value.begin(), value.end());

  this->send_bytes_value(id, data);
}

void FourHeat::send_integer_value(const std::string &id, uint32_t value) {
  std::string str = std::to_string(value);

  this->send_string_value(id, str);
}

void FourHeat::send_bool_value(const std::string &id, bool value) {
  uint8_t c = value ? '1' : '0';

  this->send_bytes_value(id, {c});
}

void FourHeat::handle_char_(uint8_t c) {
  auto now = millis();

  if (c == 0x08) {
    this->rx_message_.clear();
    return;
  }

  if (c == 0x0D) {
    this->handle_message_();

    this->rx_message_.clear();
    this->tx_message_.clear();
    this->awaiting_response_ = false;

    if (this->module_offline_) {
      this->set_module_offline_(false);
    }

    return;
  }

  this->rx_message_.push_back(c);
  this->last_rx_char_timestamp_ = now;
}

void FourHeat::handle_message_() {
  auto &data = this->rx_message_;

  if (data.size() != 18) {
    ESP_LOGW(TAG, "Received invalid data: %s", format_hex_pretty(data).c_str());
    return;
  }

  auto it = data.begin() + 6;

  if (*it == 'S' && *(it + 1) == 'Y' && *(it + 2) == 'E' && *(it + 3) == 'V' && *(it + 4) == 'O') {
    it += 5;
  }

  auto id = std::string(data.begin(), it);
  auto value = std::vector<uint8_t>(it, data.end());

  ESP_LOGV(TAG, "Received data for %s: %s", id.c_str(), format_hex_pretty(value).c_str());

  bool handled = false;

  for (const auto &listener : this->listeners_) {
    if (listener.id == id) {
      handled = true;
      listener.on_data(value);
    }
  }

  if (!handled) {
    ESP_LOGD(TAG, "Listener for %s not found. Data: %s", id.c_str(), format_hex_pretty(value).c_str());
  }
}

void FourHeat::process_next_tx_message_() {
  auto now = millis();

  if (now - this->last_tx_message_timestamp_ < TX_THROTTLE) {
    return;
  }

  if (this->awaiting_response_) {
    if (now - this->last_tx_message_timestamp_ < RESPONSE_TIMEOUT) {
      return;
    }

    this->awaiting_response_ = false;

    if (!this->module_offline_) {
      if (this->current_retry_count_ < MAX_RETRY_COUNT) {
        ESP_LOGW(TAG, "Response timeout, retrying (%d/%d)", this->current_retry_count_, MAX_RETRY_COUNT);
        this->current_retry_count_++;
      } else {
        ESP_LOGW(TAG, "Response timeout, giving up");
        this->current_retry_count_ = 0;
        this->tx_message_.clear();
        ESP_LOGW(TAG, "Clearing command TX queue, because the module can reappear in an unknown state");
        this->tx_queue_ = {};

        this->set_module_offline_(true);
      }
    }
  }

  if (this->module_offline_ && now - this->last_tx_message_timestamp_ < RETRY_THROTTLE) {
    return;
  }

  auto &data = this->tx_message_;

  if (!data.empty()) {
    ESP_LOGV(TAG, "Resending data: %s", format_hex_pretty(data).c_str());
  } else if (!this->tx_queue_.empty()) {
    data = this->tx_queue_.front();
    this->tx_queue_.pop();

    ESP_LOGV(TAG, "Sending data: %s", format_hex_pretty(data).c_str());
  } else if (this->last_sent_query_index_ < this->queries_.size()) {
    data = this->queries_[this->last_sent_query_index_];
    ESP_LOGV(TAG, "Sending query (index %d): %s", this->last_sent_query_index_, format_hex_pretty(data).c_str());

    this->last_sent_query_index_++;
  } else {
    return;
  }

  this->write_array(data);
  this->flush();

  this->last_tx_message_timestamp_ = millis();
  this->awaiting_response_ = true;
}

bool FourHeat::create_message_(const std::string &id, const std::vector<uint8_t> &value, std::vector<uint8_t> &message) {
  if (value.size() > DATA_LENGTH) {
    ESP_LOGW(TAG, "Value too long: %s", format_hex_pretty(value).c_str());
    return false;
  }

  std::vector<uint8_t> data{0x08};

  data.insert(data.end(), id.begin(), id.end());

  if (value.size() < DATA_LENGTH) {
    data.insert(data.end(), DATA_LENGTH - value.size(), '0');
  }

  data.insert(data.end(), value.begin(), value.end());

  data.push_back(0x0D);

  message = data;
  return true;
}

void FourHeat::set_module_offline_(bool offline) {
  this->module_offline_ = offline;

  if (this->module_offline_sensor_ != nullptr) {
    this->module_offline_sensor_->publish_state(offline);
  }
}

}  // namespace fourheat
}  // namespace esphome
