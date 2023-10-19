#include "esphome/core/log.h"
#include "esphome/components/fourheat/helpers.h"
#include "fourheat_switch.h"

namespace esphome {
namespace fourheat {

static const char *const TAG = "fourheat.switch";

void FourHeatSwitch::setup() {
  auto query_id = get_query_datapoint_id(this->datapoint_id_, this->query_datapoint_id_);

  this->parent_->register_query(query_id);

  this->parent_->register_listener(query_id, [this](const std::vector<uint8_t> &data) {
    this->handle_data_(data);
  });
  
  this->parent_->register_listener(this->datapoint_id_, [this](const std::vector<uint8_t> &data) {
    this->handle_data_(data);
  });
}

void FourHeatSwitch::dump_config() {
  LOG_SWITCH("", "4Heat Switch", this);
  ESP_LOGCONFIG(TAG, "  Switch has ID %s", this->datapoint_id_.c_str());
}

void FourHeatSwitch::set_fourheat_parent(FourHeat *parent) { this->parent_ = parent; }
void FourHeatSwitch::set_datapoint_id(const std::string datapoint_id) { this->datapoint_id_ = datapoint_id; }
void FourHeatSwitch::set_query_datapoint_id(const std::string datapoint_id) { this->query_datapoint_id_ = datapoint_id; }

void FourHeatSwitch::set_parser(const parser_t<bool> &parser) { this->parser_ = parser; }

void FourHeatSwitch::set_on_datapoint_id(const std::string datapoint_id) { this->on_datapoint_id_ = datapoint_id; }
void FourHeatSwitch::set_off_datapoint_id(const std::string datapoint_id) { this->off_datapoint_id_ = datapoint_id; }
void FourHeatSwitch::set_on_data(const std::string data) { this->on_data_ = data; }
void FourHeatSwitch::set_off_data(const std::string data) { this->off_data_ = data; }

void FourHeatSwitch::write_state(bool state) {
  ESP_LOGV(TAG, "Setting switch %s: %s", this->datapoint_id_.c_str(), ONOFF(state));

  std::string id;

  if (state) {
    id = this->on_datapoint_id_.value_or(this->datapoint_id_);
  } else {
    id = this->off_datapoint_id_.value_or(this->datapoint_id_);
  }

  if (state && this->on_data_.has_value()) {
    this->parent_->send_string_value(id, this->on_data_.value());
  } else if (!state && this->off_data_.has_value()) {
    this->parent_->send_string_value(id, this->off_data_.value());
  } else {
    this->parent_->send_bool_value(id, state);
  }
}

void FourHeatSwitch::handle_data_(const std::vector<uint8_t> &data) {
  auto parsed = parse<bool>(this->parser_, data);

  if (!parsed.has_value()) {
    ESP_LOGW(TAG, "Received invalid data for switch %s: %s", this->datapoint_id_.c_str(), format_hex_pretty(data).c_str());
    return;
  }

  bool value = parsed.value();

  ESP_LOGV(TAG, "4Heat reported switch %s is: %s", this->datapoint_id_.c_str(), ONOFF(value));
  this->publish_state(value);
}

}  // namespace fourheat
}  // namespace esphome
