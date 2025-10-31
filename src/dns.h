#pragma once

#include <string>
#include <string_view>

namespace dns {
// Perform reverse DNS lookup for an IP address
// Returns hostname or empty string if lookup fails
// Non-blocking: returns immediately with empty string, result cached for future calls
std::string reverse_lookup(std::string_view ip);
} // namespace dns
