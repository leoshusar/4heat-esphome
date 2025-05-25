#pragma once

#include "esphome/core/component.h"
#include "esphome/core/optional.h"
#include "esphome/components/climate/climate.h"
#include "esphome/components/fourheat/common.h"
#include "esphome/components/fourheat/fourheat.h"

namespace esphome {
namespace fourheat {

class FourHeatClimate : public climate::Climate, public Component {
 public:
  void setup() override;
  void dump_config() override;

  void set_fourheat_parent(FourHeat *parent);

  void set_datapoint_id(const std::string datapoint_id);
  void set_current_temperature_datapoint_id(const std::string datapoint_id);
  void set_target_temperature_datapoint_id(const std::string datapoint_id);
  void set_query_datapoint_id(const std::string datapoint_id);
  void set_query_current_temperature_datapoint_id(const std::string datapoint_id);
  void set_query_target_temperature_datapoint_id(const std::string datapoint_id);

  void set_parser(const parser_t<bool> &parser);
  void set_current_temperature_parser(const parser_t<int> &parser);
  void set_target_temperature_parser(const parser_t<int> &parser);
  
  void set_on_datapoint_id(const std::string datapoint_id);
  void set_off_datapoint_id(const std::string datapoint_id);
  void set_on_data(const std::string data);
  void set_off_data(const std::string data);

 protected:
  FourHeat *parent_;
  std::string datapoint_id_;
  std::string current_temperature_datapoint_id_;
  std::string target_temperature_datapoint_id_;
  esphome::optional<std::string> query_datapoint_id_;
  esphome::optional<std::string> query_current_temperature_datapoint_id_;
  esphome::optional<std::string> query_target_temperature_datapoint_id_;

  esphome::optional<parser_t<bool>> parser_;
  esphome::optional<parser_t<int>> current_temperature_parser_;
  esphome::optional<parser_t<int>> target_temperature_parser_;

  esphome::optional<std::string> on_datapoint_id_;
  esphome::optional<std::string> off_datapoint_id_;
  esphome::optional<std::string> on_data_;
  esphome::optional<std::string> off_data_;

  void control(const climate::ClimateCall &call) override;
  climate::ClimateTraits traits() override;

  void handle_state_data_(const std::vector<uint8_t> &data);
  void handle_current_temperature_data_(const std::vector<uint8_t> &data);
  void handle_target_temperature_data_(const std::vector<uint8_t> &data);
};

}  // namespace fourheat
}  // namespace esphome
