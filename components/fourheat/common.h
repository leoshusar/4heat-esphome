#pragma once

#include <ctype.h>
#include <functional>

#include "esphome/core/optional.h"

template<typename T>
using parser_t = std::function<esphome::optional<T>(const std::vector<uint8_t> &)>;
