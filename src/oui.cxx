#include "oui.h"
#include <algorithm>
#include <cassert>
#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <ranges>
#include <syncstream>

// Anonymous
namespace {

// Remove any non-hex characters from a MAC address or vendor
auto strip(std::string_view mac) {
  auto key = std::string{mac};
  key.erase(std::remove_if(key.begin(), key.end(),
                           [](char c) { return not std::isxdigit(c); }),
            key.end());
  std::transform(key.begin(), key.end(), key.begin(), ::tolower);

  assert(not key.contains(" "));
  assert(not key.contains(":"));
  assert(not key.contains(":"));

  return key;
}

// Remove any non-printable characters from a string
auto sanitise(std::string_view str) {
  auto key = std::string{str};
  key.erase(std::remove_if(key.begin(), key.end(),
                           [](char c) { return not std::isprint(c); }),
            key.end());
  return key;
}

// Tidy up a MAC address into just the vendor part
std::string mac_to_vendor(std::string_view dirty) {

  // Tidy up the incoming MAC address
  auto clean = strip(sanitise(dirty));
  auto vendor = clean.substr(0, 6);

  assert(std::size(vendor) == 6);
  return vendor;
}

namespace constd {

// use string_view literals
using namespace std::string_view_literals;

// Make it easy to replace this with a constexpr function
// Can be marked deprecated
constexpr bool is_print(char c) {

  // All printable characters
  constexpr auto printable_chars =
      "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!\"#$%&'()"
      "*+,-./:;<=>?@[\\]^_`{|}~ "sv;

  return printable_chars.contains(c);
}

static_assert(is_print(' '));
static_assert(is_print('a'));
static_assert(is_print('A'));
static_assert(is_print('!'));
static_assert(is_print('~'));
static_assert(not is_print('\n'));
static_assert(not is_print('\b'));
static_assert(not is_print('\t'));

// Check for hexadecimals
constexpr bool isxdigit(char c) {

  // All hexadecimals characters
  constexpr auto all_hex = "0123456789abcdefABCDEF"sv;

  return all_hex.contains(c);
}

static_assert(isxdigit('0'));
static_assert(isxdigit('9'));
static_assert(isxdigit('a'));
static_assert(isxdigit('f'));
static_assert(isxdigit('A'));
static_assert(isxdigit('F'));
static_assert(not isxdigit('g'));
static_assert(not isxdigit('G'));
static_assert(not isxdigit(' '));
static_assert(not isxdigit('\n'));
static_assert(not isxdigit('\b'));

} // namespace constd

// Confirm a MAC is the correct length and uses only the valid characters
// Note they can be either case, but must be consistent
constexpr bool is_valid_mac_address(std::string_view mac) {

  // No colon format
  if (std::size(mac) == 12) {
    // Only hexadecimals allowed
    if (std::ranges::all_of(mac, [](char c) { return constd::isxdigit(c); }))
      return true;
  }

  // Colon format
  else if (std::size(mac) == 17) {
    // Only hexadecimals and colons allowed
    if (std::ranges::all_of(
            mac, [](char c) { return constd::isxdigit(c) or c == ':'; }))
      return true;
  }

  // Not a valid MAC address
  return false;
}

// Colon format
static_assert(is_valid_mac_address("00:00:00:00:00:00"));
static_assert(is_valid_mac_address("ff:ff:ff:ff:ff:ff"));

// No colon format
static_assert(is_valid_mac_address("000000000000"));
static_assert(is_valid_mac_address("ffffffffffff"));

// Invalid formats
static_assert(not is_valid_mac_address(""));
static_assert(not is_valid_mac_address("00:00:00:00:"));
static_assert(not is_valid_mac_address("00:00:00:00:00:00:0 "));
static_assert(not is_valid_mac_address("\n\b\n\b\n\b\n\b\n\b\n\b"));
static_assert(not is_valid_mac_address("00:00:00:00:00:00:00"));
static_assert(not is_valid_mac_address("00:00:00:00:00"));
static_assert(not is_valid_mac_address("00:00:00:00:00:00:00:00"));
static_assert(not is_valid_mac_address("00:\n00:00:00:00:00"));
static_assert(not is_valid_mac_address("00\n0000000000"));

// Create the OUI database from a text file
std::map<std::string, std::string> get_oui() {

  // auto oui_file_locations = std::array<std::string>{
  //     "/usr/share/arp-scan/ieee-oui.txt",
  //     "out.txt"};

  // auto preferred_oui = std::find_if(oui_file_locations.begin(),
  //                               oui_file_locations.end(), [](auto &file) {
  //                                 return std::ifstream{file}.good();
  //                               });

  // Open the first file that exists

  auto in = std::ifstream{"ieee-oui.txt"};

  // Read whole file into a string
  auto str = std::string{std::istreambuf_iterator<char>{in},
                         std::istreambuf_iterator<char>{}};

  // Initialise database with some vendors that are missing from the oui.txt
  auto oui = std::map<std::string, std::string>{
      {"f2ed07", "Nothing Technology Limited"},
      {"3e0692", "Nothing Technology Limited"},
      {"01005e", "IPv4 multicast"},
      {"0180c2", "IEEE 802.1X"},
      {"333300", "IPv6 multicast"},
      {"3333ff", "IPv6 multicast"},
      {"ffffff", "Broadcast"},
      {"000000", "Unicast"}};

  // Parse each line
  for (auto line : str | std::views::split('\n')) {

    // Look for all the lines with the (hex) string
    auto s = std::string{line.begin(), line.end()};
    if (not s.contains("(hex)") or s.empty())
      continue;

    auto pos = s.find("(hex)");

    if (pos != std::string::npos) {

      auto key = s.substr(0, 8);
      auto value = s.substr(pos + 6);

      // Remove non-hex characters before using the key
      auto vendor = strip(key);

      oui[vendor] = value;
    }
  }

  return oui;
}

// Initialise the database on startup
const auto database = get_oui();
} // namespace

// Lookup a MAC address in the database
namespace oui {
std::string lookup(std::string_view mac) {

  // Create key from MAC address
  auto vendor = mac_to_vendor(mac);

  // Search for the vendor in the database
  auto it = database.find(vendor);
  auto details = it != database.end() ? it->second : "";

  // Return the cleaned up vendor details if found
  // Otherwise just return the MAC address
  return std::empty(details) ? std::string{mac} : sanitise(details);
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