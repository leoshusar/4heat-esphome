#pragma once

#include <ctype.h>
#include <functional>
#include <optional>

template<typename T>
using parser_t = std::function<std::optional<T>(const std::vector<uint8_t> &)>;
