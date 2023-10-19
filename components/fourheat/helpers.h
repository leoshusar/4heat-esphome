#pragma once

#include "common.h"

#include <ctype.h>
#include <functional>
#include <optional>
#include <stdint.h>
#include <string>
#include <vector>

namespace esphome {
namespace fourheat {

bool parse_bool(const std::vector<uint8_t> &data, bool &value);
bool parse_int(const std::vector<uint8_t> &data, int &value);

const std::string get_query_datapoint_id(const std::string &datapoint_id, const std::optional<std::string> &query_datapoint_id);

template<typename T>
std::optional<T> parse(const std::optional<parser_t<T>> &parser, const std::vector<uint8_t> &data);

}  // namespace fourheat
}  // namespace esphome
