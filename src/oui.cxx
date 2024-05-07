#include "oui.h"
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <ranges>

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

  if (std::size(vendor) != 6) {

    std::cout << "Invalid MAC address: " << dirty << std::endl;
    abort();
  }

  assert(std::size(vendor) < 7);
  return vendor;
}

// Create the OUI database from a text file
std::map<std::string, std::string> get_oui() {

  auto in = std::ifstream{"oui.txt"};

  // Read whole file into a string
  auto str = std::string{std::istreambuf_iterator<char>{in},
                         std::istreambuf_iterator<char>{}};

  // Initialise the database with come common vendors that are missing from the
  // oui.txt file
  auto oui = std::map<std::string, std::string>{
      {"f2ed07", "Nothing Technology Limited"},
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