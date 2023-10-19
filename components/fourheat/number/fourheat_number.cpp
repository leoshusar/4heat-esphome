#include "esphome/core/log.h"
#include "esphome/components/fourheat/helpers.h"
#include "fourheat_number.h"

namespace esphome {
namespace fourheat {

static const char *const TAG = "fourheat.number";

void FourHeatNumber::setup() {
  auto query_id = get_query_datapoint_id(this->datapoint_id_, this->query_datapoint_id_);

  this->parent_->register_query(query_id);

  this->parent_->register_listener(query_id, [this](const std::vector<uint8_t> &data) {
    this->handle_data_(data);
  });

  this->parent_->register_listener(this->datapoint_id_, [this](const std::vector<uint8_t> &data) {
    this->handle_data_(data);
  });
}

void FourHeatNumber::dump_config() {
  LOG_NUMBER("", "4Heat Number", this);
  ESP_LOGCONFIG(TAG, "  Number has ID %s", this->datapoint_id_.c_str());
}

void FourHeatNumber::set_fourheat_parent(FourHeat *parent) { this->parent_ = parent; }
void FourHeatNumber::set_datapoint_id(const std::string datapoint_id) { this->datapoint_id_ = datapoint_id; }
void FourHeatNumber::set_query_datapoint_id(const std::string datapoint_id) { this->query_datapoint_id_ = datapoint_id; }

void FourHeatNumber::set_parser(const parser_t<int> &parser) { this->parser_ = parser; }

void FourHeatNumber::control(float value) {
  ESP_LOGV(TAG, "Setting number %s: %f", this->datapoint_id_.c_str(), value);

  int integer_value = lround(value);

  this->parent_->send_integer_value(this->datapoint_id_, integer_value);
  this->publish_state(value);
}

void FourHeatNumber::handle_data_(const std::vector<uint8_t> &data) {
  auto parsed = parse<int>(this->parser_, data);

  if (!parsed.has_value()) {
    ESP_LOGW(TAG, "Received invalid data for number %s: %s", this->datapoint_id_.c_str(), format_hex_pretty(data).c_str());
    return;
  }

  int value = parsed.value();

  ESP_LOGV(TAG, "4Heat reported number %s is: %d", this->datapoint_id_.c_str(), value);
  this->publish_state(value);
}

}  // namespace fourheat
}  // namespace esphome
