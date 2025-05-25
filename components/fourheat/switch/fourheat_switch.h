#pragma once

#include "esphome/core/component.h"
#include "esphome/core/optional.h"
#include "esphome/components/fourheat/common.h"
#include "esphome/components/fourheat/fourheat.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace fourheat {

class FourHeatSwitch : public switch_::Switch, public Component {
 public:
  void setup() override;
  void dump_config() override;

  void set_fourheat_parent(FourHeat *parent);
  void set_datapoint_id(const std::string datapoint_id);
  void set_query_datapoint_id(const std::string datapoint_id);

  void set_parser(const parser_t<bool> &parser);

  void set_on_datapoint_id(const std::string datapoint_id);
  void set_off_datapoint_id(const std::string datapoint_id);
  void set_on_data(const std::string data);
  void set_off_data(const std::string data);

 protected:
  FourHeat *parent_;
  std::string datapoint_id_;
  esphome::optional<std::string> query_datapoint_id_;

  esphome::optional<parser_t<bool>> parser_;

  optional<std::string> on_datapoint_id_;
  optional<std::string> off_datapoint_id_;
  optional<std::string> on_data_;
  optional<std::string> off_data_;

  void write_state(bool state) override;

  void handle_data_(const std::vector<uint8_t> &data);
};

}  // namespace fourheat
}  // namespace esphome
