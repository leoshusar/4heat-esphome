#include "esphome/core/log.h"
#include "esphome/components/fourheat/helpers.h"
#include "fourheat_text_sensor.h"

namespace esphome {
namespace fourheat {

static const char *const TAG = "fourheat.text_sensor";

void FourHeatTextSensor::setup() {
  std::string id = get_query_datapoint_id(this->datapoint_id_, this->query_datapoint_id_);

  this->parent_->register_query(id);

  this->parent_->register_listener(this->datapoint_id_, [this](const std::vector<uint8_t> &data) {
    this->handle_data_(data);
  });
}

void FourHeatTextSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "4Heat Text Sensor:");
  ESP_LOGCONFIG(TAG, "  Text Sensor has ID %s", this->datapoint_id_.c_str());
}

void FourHeatTextSensor::set_fourheat_parent(FourHeat *parent) { this->parent_ = parent; }
void FourHeatTextSensor::set_datapoint_id(const std::string datapoint_id) { this->datapoint_id_ = datapoint_id; }
void FourHeatTextSensor::set_query_datapoint_id(const std::string datapoint_id) { this->query_datapoint_id_ = datapoint_id; }

void FourHeatTextSensor::set_parser(const parser_t<int> &parser) { this->parser_ = parser; }

void FourHeatTextSensor::set_options(const std::map<int, std::string> options) { this->options_ = std::move(options); }

void FourHeatTextSensor::handle_data_(const std::vector<uint8_t> &data) {
  auto parsed = parse<int>(this->parser_, data);

  if (!parsed.has_value()) {
    ESP_LOGW(TAG, "Received invalid data for text sensor %s: %s", this->datapoint_id_.c_str(), format_hex_pretty(data).c_str());
    return;
  }

  int value = parsed.value();

  if (this->options_.find(value) == this->options_.end()) {
    ESP_LOGW(TAG, "Received invalid value for text sensor %s: %d", this->datapoint_id_.c_str(), value);
    return;
  }

  this->publish_state(this->options_[value]);
}

}  // namespace fourheat
}  // namespace esphome

