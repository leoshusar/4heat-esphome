#include "esphome/core/log.h"
#include "esphome/components/fourheat/helpers.h"
#include "fourheat_sensor.h"

namespace esphome {
namespace fourheat {

static const char *const TAG = "fourheat.sensor";

void FourHeatSensor::setup() {
  auto query_id = get_query_datapoint_id(this->datapoint_id_, this->query_datapoint_id_);

  this->parent_->register_query(query_id);

  this->parent_->register_listener(this->datapoint_id_, [this](const std::vector<uint8_t> &data) {
    this->handle_data_(data);
  });
}

void FourHeatSensor::dump_config() {
  LOG_SENSOR("", "4Heat Sensor", this);
  ESP_LOGCONFIG(TAG, "  Sensor has ID %s", this->datapoint_id_.c_str());
}

void FourHeatSensor::set_fourheat_parent(FourHeat *parent) { this->parent_ = parent; }
void FourHeatSensor::set_datapoint_id(const std::string datapoint_id) { this->datapoint_id_ = datapoint_id; }
void FourHeatSensor::set_query_datapoint_id(const std::string datapoint_id) { this->query_datapoint_id_ = datapoint_id; }

void FourHeatSensor::set_parser(const parser_t<int> &parser) { this->parser_ = parser; }

void FourHeatSensor::handle_data_(const std::vector<uint8_t> &data) {
  auto parsed = parse<int>(this->parser_, data);

  if (!parsed.has_value()) {
    ESP_LOGW(TAG, "Received invalid data for sensor %s: %s", this->datapoint_id_.c_str(), format_hex_pretty(data).c_str());
    return;
  }

  int value = parsed.value();

  ESP_LOGV(TAG, "4Heat reported sensor %s is: %d", this->datapoint_id_.c_str(), value);
  this->publish_state(value);
}

}  // namespace fourheat
}  // namespace esphome
