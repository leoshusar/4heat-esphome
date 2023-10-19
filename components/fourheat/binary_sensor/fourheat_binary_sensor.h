#pragma once

#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/fourheat/common.h"
#include "esphome/components/fourheat/fourheat.h"

#include <optional>

namespace esphome {
namespace fourheat {

class FourHeatBinarySensor : public binary_sensor::BinarySensor, public Component {
 public:
  void setup() override;
  void dump_config() override;
  
  void set_fourheat_parent(FourHeat *parent);
  void set_datapoint_id(const std::string datapoint_id);
  void set_query_datapoint_id(const std::string datapoint_id);

  void set_parser(const parser_t<bool> &parser);

 protected:
  FourHeat *parent_;
  std::string datapoint_id_;
  std::optional<std::string> query_datapoint_id_;

  std::optional<parser_t<bool>> parser_;

  void handle_data_(const std::vector<uint8_t> &data);
};

}  // namespace fourheat
}  // namespace esphome
