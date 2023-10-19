#include "esphome/core/log.h"
#include "esphome/components/fourheat/helpers.h"
#include "fourheat_select.h"

namespace esphome {
namespace fourheat {

static const char *const TAG = "fourheat.select";

void FourHeatSelect::setup() {
  auto query_id = get_query_datapoint_id(this->datapoint_id_, this->query_datapoint_id_);

  this->parent_->register_query(query_id);

  this->parent_->register_listener(query_id, [this](const std::vector<uint8_t> &data) {
    this->handle_data_(data);
  });

  this->parent_->register_listener(this->datapoint_id_, [this](const std::vector<uint8_t> &data) {
    this->handle_data_(data);
  });
}

void FourHeatSelect::dump_config() {
  LOG_SELECT("", "4Heat Select", this);
  ESP_LOGCONFIG(TAG, "  Select has ID %s", this->datapoint_id_.c_str());
  ESP_LOGCONFIG(TAG, "  Options are:");
  for (const auto &option : this->options_) {
    ESP_LOGCONFIG(TAG, "    %i: %s", option.first, option.second.c_str());
  }
}

void FourHeatSelect::set_fourheat_parent(FourHeat *parent) { this->parent_ = parent; }
void FourHeatSelect::set_optimistic(bool optimistic) { this->optimistic_ = optimistic; }
void FourHeatSelect::set_datapoint_id(const std::string datapoint_id) { this->datapoint_id_ = datapoint_id; }
void FourHeatSelect::set_query_datapoint_id(const std::string datapoint_id) { this->query_datapoint_id_ = datapoint_id; }

void FourHeatSelect::set_parser(const parser_t<int> &parser) { this->parser_ = parser; }

void FourHeatSelect::set_options(const std::map<int, std::string> options) { this->options_ = std::move(options); }

void FourHeatSelect::control(const std::string &value) {
  auto it = std::find_if(this->options_.begin(), this->options_.end(), [&value](const std::pair<int, std::string> &p) {
    return p.second == value;
  });

  if (it == this->options_.end()) {
    ESP_LOGW(TAG, "Invalid value %s", value.c_str());
    return;
  }

  int key = it->first;
  ESP_LOGV(TAG, "Setting select %s to %d: %s", this->datapoint_id_.c_str(), key, value.c_str());
  this->parent_->send_integer_value(this->datapoint_id_, key);
  
  if (this->optimistic_)
    this->publish_state(value);
}

void FourHeatSelect::handle_data_(const std::vector<uint8_t> &data) {
  auto parsed = parse<int>(this->parser_, data);

  if (!parsed.has_value()) {
    ESP_LOGW(TAG, "Received invalid data for select %s: %s", this->datapoint_id_.c_str(), format_hex_pretty(data).c_str());
    return;
  }

  int value = parsed.value();

  if (this->options_.find(value) == this->options_.end()) {
    ESP_LOGW(TAG, "Received invalid value for select %s: %d", this->datapoint_id_.c_str(), value);
    this->publish_state("unknown");
    return;
  }

  ESP_LOGV(TAG, "4Heat reported select %s is: %d", this->datapoint_id_.c_str(), value);
  this->publish_state(this->options_[value]);
}

}  // namespace fourheat
}  // namespace esphome

