#pragma once

#include <string>
#include <string_view>

// Vendor lookup based on MAC address

namespace oui {
std::string lookup(const std::string_view);
}
