#pragma once

#include "esphome/core/component.h"
#include "esphome/components/button/button.h"
#include "esphome/components/fourheat/fourheat.h"

namespace esphome {
namespace fourheat {

class FourHeatButton : public button::Button, public Component {
 public:
  void dump_config() override;
  
  void set_fourheat_parent(FourHeat *parent);
  void set_datapoint_id(const std::string datapoint_id);

  void set_press_data(const std::string press_data);

 protected:
  FourHeat *parent_;
  std::string datapoint_id_;
  std::string press_data_{"1"};

  void press_action() override;
};

}  // namespace fourheat
}  // namespace esphome
