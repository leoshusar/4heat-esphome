#include "esphome/core/log.h"
#include "esphome/components/fourheat/helpers.h"
#include "fourheat_climate.h"

namespace esphome {
namespace fourheat {

static const char *const TAG = "fourheat.climate";

void FourHeatClimate::setup() {
  auto state_id = get_query_datapoint_id(this->datapoint_id_, this->query_datapoint_id_);
  this->parent_->register_query(state_id);

  auto current_temp_id = get_query_datapoint_id(this->current_temperature_datapoint_id_, this->query_current_temperature_datapoint_id_);
  this->parent_->register_query(current_temp_id);

  auto target_temp_id = get_query_datapoint_id(this->target_temperature_datapoint_id_, this->query_target_temperature_datapoint_id_);
  this->parent_->register_query(target_temp_id);

  this->parent_->register_listener(this->datapoint_id_, [this](const std::vector<uint8_t> &data) {
    this->handle_state_data_(data);
  });

  this->parent_->register_listener(this->current_temperature_datapoint_id_, [this](const std::vector<uint8_t> &data) {
    this->handle_current_temperature_data_(data);
  });

  this->parent_->register_listener(this->target_temperature_datapoint_id_, [this](const std::vector<uint8_t> &data) {
    this->handle_target_temperature_data_(data);
  });
}

void FourHeatClimate::dump_config() {
  LOG_CLIMATE("", "4Heat Climate", this);
  ESP_LOGCONFIG(TAG, "  Climate has ID %s", this->datapoint_id_.c_str());
}

void FourHeatClimate::set_fourheat_parent(FourHeat *parent) { this->parent_ = parent; }

void FourHeatClimate::set_datapoint_id(const std::string datapoint_id) { this->datapoint_id_ = datapoint_id; }
void FourHeatClimate::set_current_temperature_datapoint_id(const std::string datapoint_id) { this->current_temperature_datapoint_id_ = datapoint_id; }
void FourHeatClimate::set_target_temperature_datapoint_id(const std::string datapoint_id) { this->target_temperature_datapoint_id_ = datapoint_id; }
void FourHeatClimate::set_query_datapoint_id(const std::string datapoint_id) { this->query_datapoint_id_ = datapoint_id; }
void FourHeatClimate::set_query_current_temperature_datapoint_id(const std::string datapoint_id) { this->query_current_temperature_datapoint_id_ = datapoint_id; }
void FourHeatClimate::set_query_target_temperature_datapoint_id(const std::string datapoint_id) { this->query_target_temperature_datapoint_id_ = datapoint_id; }

void FourHeatClimate::set_parser(const parser_t<bool> &parser) { this->parser_ = parser; }
void FourHeatClimate::set_current_temperature_parser(const parser_t<int> &parser) { this->current_temperature_parser_ = parser; }
void FourHeatClimate::set_target_temperature_parser(const parser_t<int> &parser) { this->target_temperature_parser_ = parser; }

void FourHeatClimate::set_on_datapoint_id(const std::string datapoint_id) { this->on_datapoint_id_ = datapoint_id; }
void FourHeatClimate::set_off_datapoint_id(const std::string datapoint_id) { this->off_datapoint_id_ = datapoint_id; }
void FourHeatClimate::set_on_data(const std::string data) { this->on_data_ = data; }
void FourHeatClimate::set_off_data(const std::string data) { this->off_data_ = data; }

void FourHeatClimate::control(const climate::ClimateCall &call) {
  if (call.get_mode().has_value()) {
    const bool switch_state = *call.get_mode() != climate::CLIMATE_MODE_OFF;

    ESP_LOGV(TAG, "Setting state: %s", ONOFF(switch_state));

    std::string id;

    if (switch_state) {
      id = this->on_datapoint_id_.value_or(this->datapoint_id_);
    } else {
      id = this->off_datapoint_id_.value_or(this->datapoint_id_);
    }

    if (switch_state && this->on_data_.has_value()) {
      this->parent_->send_string_value(id, this->on_data_.value());
    } else if (!switch_state && this->off_data_.has_value()) {
      this->parent_->send_string_value(id, this->off_data_.value());
    } else {
      this->parent_->send_bool_value(id, switch_state);
    }

    this->parent_->send_bool_value(this->datapoint_id_, switch_state);
  }

  if (call.get_target_temperature().has_value()) {
    float target_temperature = *call.get_target_temperature();
    int target_temperature_int = lround(target_temperature);

    ESP_LOGV(TAG, "Setting target temperature: %.1f", target_temperature);
    this->parent_->send_integer_value(this->target_temperature_datapoint_id_, target_temperature_int);
  }
}

climate::ClimateTraits FourHeatClimate::traits() {
  auto traits = climate::ClimateTraits();
  traits.set_supports_action(true);
  traits.set_supports_current_temperature(true);
  traits.add_supported_mode(climate::CLIMATE_MODE_HEAT);
  traits.add_supported_mode(climate::CLIMATE_MODE_OFF);
  return traits;
}

void FourHeatClimate::handle_state_data_(const std::vector<uint8_t> &data) {
  auto parsed = parse<bool>(this->parser_, data);

  if (!parsed.has_value()) {
    ESP_LOGW(TAG, "Received invalid data for climate %s: %s", this->datapoint_id_.c_str(), format_hex_pretty(data).c_str());
    return;
  }

  bool value = parsed.value();

  ESP_LOGV(TAG, "4Heat reported climate state %s is: %d", this->datapoint_id_.c_str(), value);
  this->action = value ? climate::CLIMATE_ACTION_HEATING : climate::CLIMATE_ACTION_OFF;

  this->publish_state();
}

void FourHeatClimate::handle_current_temperature_data_(const std::vector<uint8_t> &data) {
  auto parsed = parse<int>(this->current_temperature_parser_, data);

  if (!parsed.has_value()) {
    ESP_LOGW(TAG, "Received invalid data for climate %s: %s", this->current_temperature_datapoint_id_.c_str(), format_hex_pretty(data).c_str());
    return;
  }

  int value = parsed.value();

  ESP_LOGV(TAG, "4Heat reported current temperature %s is: %d", this->current_temperature_datapoint_id_.c_str(), value);
  this->current_temperature = value;

  this->publish_state();
}

void FourHeatClimate::handle_target_temperature_data_(const std::vector<uint8_t> &data) {
  auto parsed = parse<int>(this->target_temperature_parser_, data);

  if (!parsed.has_value()) {
    ESP_LOGW(TAG, "Received invalid data for climate %s: %s", this->target_temperature_datapoint_id_.c_str(), format_hex_pretty(data).c_str());
    return;
  }

  int value = parsed.value();

  ESP_LOGV(TAG, "4Heat reported target temperature %s is: %d", this->target_temperature_datapoint_id_.c_str(), value);
  this->target_temperature = value;

  this->publish_state();
}

}  // namespace fourheat
}  // namespace esphome
