#include "oui.h"
#include <algorithm>
#include <cassert>
#include <format>
#include <fstream>
#include <map>
#include <ranges>
#include <string>
#include <string_view>

// Anonymous namespace
namespace {

// Remove any non-hex characters from a MAC address or vendor
auto strip(std::string_view mac) {
  auto stripped = std::string{mac};
  stripped.erase(std::remove_if(stripped.begin(), stripped.end(),
                                [](char c) { return not std::isxdigit(c); }),
                 stripped.end());

  assert(not stripped.contains(" "));
  assert(not stripped.contains(":"));
  assert(not stripped.contains(":"));

  return stripped;
}

// Remove any non-printable characters from a string
auto sanitise(std::string_view str) {
  auto key = std::string{str};
  key.erase(std::remove_if(key.begin(), key.end(),
                           [](char c) { return not std::isprint(c); }),
            key.end());
  return key;
}

using namespace std::string_view_literals;

// Get key/value pairs from an entry in the OUI database
// Otherwise return a pair of empty strings
constexpr std::pair<std::string, std::string>
get_vendor_desc(std::string_view line) {

  auto out = std::pair<std::string, std::string>{};

  // Find the first space
  auto space = line.find_first_of(" \t");

  // If no space, return empty strings
  if (space == std::string::npos)
    return out;

  // Otherwise split on the first space
  auto key = line.substr(0, space);
  auto value = line.substr(space + 1);

  // Trim any leading or trailing spaces
  value.remove_prefix(std::min(value.find_first_not_of(' '), value.size()));

  out = {std::string{key}, std::string{value}};

  return out;
}

static_assert(get_vendor_desc("a b") == std::pair{"a"sv, "b"sv});
static_assert(get_vendor_desc("a  b") == std::pair{"a"sv, "b"sv});
static_assert(get_vendor_desc("a\tb") == std::pair{"a"sv, "b"sv});
static_assert(get_vendor_desc("00:00:00:00:00:00      Unicast") ==
              std::pair{"00:00:00:00:00:00"sv, "Unicast"sv});
static_assert(get_vendor_desc("00:00:00:00:00:00") == std::pair{""sv, ""sv});

// Create the OUI database from a text file
std::map<std::string, std::string> get_oui() {

  // Open the system vendor file
  auto in = std::ifstream{"/usr/share/arp-scan/ieee-oui.txt"};

  // Read whole file into a string
  auto str = std::string{std::istreambuf_iterator<char>{in},
                         std::istreambuf_iterator<char>{}};

  // The vendor database
  auto oui = std::map<std::string, std::string>{};

  // Parse each line
  for (auto line : str | std::views::split('\n')) {

    if (line.empty())
      continue;

    if (line.front() == '#')
      continue;

    auto [key, value] = get_vendor_desc(std::string{line.begin(), line.end()});

    if (key.empty())
      continue;

    oui[std::string{key}] = std::string{value};
  }

  return oui;
}

// Initialise the database on startup
const auto database = get_oui();

} // namespace

// Lookup a MAC address in the database
namespace oui {
std::string lookup(std::string_view mac) {

  auto vendor = std::string{"unknown"};

  // Remove any non-hex characters from the MAC address
  auto key2 = strip(mac);

  // Make uppercase
  std::transform(key2.begin(), key2.end(), key2.begin(), ::toupper);

  // Match the largest portion of the MAC address in the database
  while (std::size(key2) > 5) {

    // Search for the vendor in the database
    if (database.contains(key2))
      return sanitise(database.at(key2));

    // Remove the last character
    key2.pop_back();
  }

  return {};
}

// Pretty print the MAC address
std::string prettify(const std::string_view dirty) {

  // Tidy up the incoming MAC address
  auto clean = strip(sanitise(dirty));

  // Create a pretty version of the MAC address
  auto pretty = std::string{};

  // Insert colons every two characters
  for (auto x : clean | std::views::split(2)) {
    pretty += std::string{x.begin(), x.end()};
    pretty += ":";
  }

  // Remove the trailing colon
  pretty.pop_back();

  return pretty;
}
} // namespace oui