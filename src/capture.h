#pragma once
#include "device.h"
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace cap {
std::multimap<std::string, device_t> read(std::string_view);
std::vector<std::string> interfaces();
} // namespace cap