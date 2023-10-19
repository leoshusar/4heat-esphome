#pragma once

#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/uart/uart.h"

#include <queue>

namespace esphome {
namespace fourheat {

struct FourHeatDataListener {
  std::string id;
  std::function<void(const std::vector<uint8_t> &)> on_data;
};

class FourHeat : public PollingComponent, public uart::UARTDevice {
 public:
  float get_setup_priority() const override { return setup_priority::LATE; }

  void loop() override;
  void update() override;
  void dump_config() override;

  void set_module_offline_sensor(binary_sensor::BinarySensor *module_offline_sensor);
  void register_query(const std::string &id);
  void register_listener(const std::string &id, const std::function<void(const std::vector<uint8_t>)> &func);

  void send_raw_value(const std::vector<uint8_t> &value);
  void send_bytes_value(const std::string &id, const std::vector<uint8_t> &value);
  void send_string_value(const std::string &id, const std::string &value);
  void send_integer_value(const std::string &id, uint32_t value);
  void send_bool_value(const std::string &id, bool value);

 protected:
  binary_sensor::BinarySensor *module_offline_sensor_{nullptr};

  uint32_t last_rx_char_timestamp_{0};
  uint32_t last_tx_message_timestamp_{0};
  std::vector<uint8_t> rx_message_;

  bool awaiting_response_{false};
  bool module_offline_{true};
  uint8_t current_retry_count_{0};
  uint8_t last_sent_query_index_{0};
  std::vector<uint8_t> tx_message_;

  std::vector<std::vector<uint8_t>> queries_;
  std::queue<std::vector<uint8_t>> tx_queue_;
  std::vector<FourHeatDataListener> listeners_;

  void handle_char_(uint8_t c);
  void handle_message_();
  void process_next_tx_message_();

  bool create_message_(const std::string &id, const std::vector<uint8_t> &value, std::vector<uint8_t> &message);
  void set_module_offline_(bool offline);
};

}  // namespace fourheat
}  // namespace esphome
