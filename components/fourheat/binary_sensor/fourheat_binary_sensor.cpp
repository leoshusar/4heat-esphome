#include "esphome/core/log.h"
#include "esphome/components/fourheat/helpers.h"
#include "fourheat_binary_sensor.h"

namespace esphome {
namespace fourheat {

static const char *const TAG = "fourheat.binary_sensor";

void FourHeatBinarySensor::setup() {
  auto query_id = get_query_datapoint_id(this->datapoint_id_, this->query_datapoint_id_);

  this->parent_->register_query(query_id);

  this->parent_->register_listener(this->datapoint_id_, [this](const std::vector<uint8_t> &data) {
    this->handle_data_(data);
  });
}

void FourHeatBinarySensor::dump_config() {
  ESP_LOGCONFIG(TAG, "4Heat Binary Sensor:");
  ESP_LOGCONFIG(TAG, "  Binary Sensor has ID %s", this->datapoint_id_.c_str());
}

void FourHeatBinarySensor::set_fourheat_parent(FourHeat *parent) { this->parent_ = parent; }
void FourHeatBinarySensor::set_datapoint_id(const std::string datapoint_id) { this->datapoint_id_ = datapoint_id; }
void FourHeatBinarySensor::set_query_datapoint_id(const std::string datapoint_id) { this->query_datapoint_id_ = datapoint_id; }

void FourHeatBinarySensor::set_parser(const parser_t<bool> &parser) { this->parser_ = parser; }

void FourHeatBinarySensor::handle_data_(const std::vector<uint8_t> &data) {
  auto parsed = parse<bool>(this->parser_, data);

  if (!parsed.has_value()) {
    ESP_LOGW(TAG, "Received invalid data for binary sensor %s: %s", this->datapoint_id_.c_str(), format_hex_pretty(data).c_str());
    return;
  }

  bool value = parsed.value();

  ESP_LOGV(TAG, "4Heat reported binary sensor %s is: %s", this->datapoint_id_.c_str(), ONOFF(value));
  this->publish_state(value);
}

}  // namespace fourheat
}  // namespace esphome
