#pragma once

#include "esphome/core/component.h"
#include "esphome/components/fourheat/fourheat.h"
#include "esphome/components/text_sensor/text_sensor.h"

#include <optional>

namespace esphome {
namespace fourheat {

class FourHeatTextSensor : public text_sensor::TextSensor, public Component {
 public:
  void setup() override;
  void dump_config() override;

  void set_fourheat_parent(FourHeat *parent);
  void set_datapoint_id(const std::string datapoint_id);
  void set_query_datapoint_id(const std::string datapoint_id);

  void set_parser(const parser_t<int> &parser);

  void set_options(const std::map<int, std::string> options);

 protected:
  FourHeat *parent_;
  std::string datapoint_id_;
  std::optional<std::string> query_datapoint_id_;

  std::optional<parser_t<int>> parser_;

  std::map<int, std::string> options_;

  void handle_data_(const std::vector<uint8_t> &data);
};

}  // namespace fourheat
}  // namespace esphome
