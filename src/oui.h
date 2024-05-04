#pragma once

#include <string>
#include <string_view>

namespace oui {
// Take a MAC address, sanitise it and return the vendor details
std::string lookup(const std::string_view);

// Pretty print a MAC address
std::string prettify(const std::string_view);
} // namespace oui
