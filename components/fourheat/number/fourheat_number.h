#pragma once

#include "esphome/core/component.h"
#include "esphome/core/optional.h"
#include "esphome/components/fourheat/common.h"
#include "esphome/components/fourheat/fourheat.h"
#include "esphome/components/number/number.h"

namespace esphome {
namespace fourheat {

class FourHeatNumber : public number::Number, public Component {
 public:
  void setup() override;
  void dump_config() override;
  
  void set_fourheat_parent(FourHeat *parent);
  void set_datapoint_id(const std::string datapoint_id);
  void set_query_datapoint_id(const std::string datapoint_id);

  void set_parser(const parser_t<int> &parser);

 protected:
  FourHeat *parent_;
  std::string datapoint_id_;
  esphome::optional<std::string> query_datapoint_id_;

  esphome::optional<parser_t<int>> parser_;

  void control(float value) override;
  
  void handle_data_(const std::vector<uint8_t> &data);
};

}  // namespace fourheat
}  // namespace esphome
