#include "helpers.h"

namespace esphome {
namespace fourheat {

bool parse_bool(const std::vector<uint8_t> &data, bool &value) {
  auto size = data.size();

  for (int i = 0; i < size - 2; i++) {
    if (data[i] != '0') {
      return false;
    }
  }

  uint8_t byte = data[size - 1] - '0';

  if (byte > 1) {
    return false;
  }

  value = byte;
  return true;
}

bool parse_int(const std::vector<uint8_t> &data, int &value) {
  for (const uint8_t b : data) {
    if (!isdigit(b)) {
      return false;
    }
  }

  std::string int_str(data.begin(), data.end());
  value = stoi(int_str);
  return true;
}

const std::string get_query_datapoint_id(const std::string &datapoint_id, const esphome::optional<std::string> &query_datapoint_id) {
  if (query_datapoint_id.has_value()) {
    return query_datapoint_id.value();
  }

  std::string query_datapoint_id_str = datapoint_id;
  query_datapoint_id_str[0]--;

  return query_datapoint_id_str;
}

template<>
esphome::optional<bool> parse(const esphome::optional<parser_t<bool>> &parser, const std::vector<uint8_t> &data) {
  if (parser.has_value()) {
    return parser.value()(data);
  }

  bool value;

  if (!parse_bool(data, value)) {
    return esphome::nullopt;
  }

  return value;
}

template<>
esphome::optional<int> parse(const esphome::optional<parser_t<int>> &parser, const std::vector<uint8_t> &data) {
  if (parser.has_value()) {
    return parser.value()(data);
  }

  int value;

  if (!parse_int(data, value)) {
    return esphome::nullopt;
  }

  return value;
}

// bool parse_float(const std::vector<uint8_t> &data, float &value) {
//   if (!validate_number(data)) {
//     return false;
//   }

//   std::string float_str(data.begin(), data.end());
//   value = stof(float_str);
//   return true;
// }

}  // namespace fourheat
}  // namespace esphome
