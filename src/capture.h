#pragma once

#include "device.h"
#include <string>
#include <vector>

// List network interfaces
namespace cap {
std::vector<std::string> interfaces();
} // namespace cap