#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <map>
#include "device.h"

namespace cap {
std::multimap<std::string, device_t> read(std::string_view);
std::vector<std::string> interfaces();
} // namespace cap