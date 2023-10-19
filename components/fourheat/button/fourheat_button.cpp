#include "esphome/core/log.h"
#include "esphome/components/fourheat/helpers.h"
#include "fourheat_button.h"

namespace esphome {
namespace fourheat {

static const char *const TAG = "fourheat.button";

void FourHeatButton::dump_config() {
  ESP_LOGCONFIG(TAG, "4Heat Button:");
  ESP_LOGCONFIG(TAG, "  Button has ID %s", this->datapoint_id_.c_str());
}

void FourHeatButton::set_fourheat_parent(FourHeat *parent) { this->parent_ = parent; }
void FourHeatButton::set_datapoint_id(const std::string datapoint_id) { this->datapoint_id_ = datapoint_id; }

void FourHeatButton::set_press_data(const std::string press_data) { this->press_data_ = press_data; }

void FourHeatButton::press_action() {
  ESP_LOGV(TAG, "Button %s pressed", this->datapoint_id_.c_str());
  this->parent_->send_string_value(this->datapoint_id_, this->press_data_);
}

}  // namespace fourheat
}  // namespace esphome
