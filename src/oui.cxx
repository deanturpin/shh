#include <algorithm>
#include <fstream>
#include <map>
#include <ranges>
#include "oui.h"

// Anonymous
namespace {

// Create the OUI database from a text file
std::map<std::string, std::string> get_oui() {

  auto in = std::ifstream{"oui.txt"};

  // Read whole file into a string
  auto str = std::string{std::istreambuf_iterator<char>{in},
                         std::istreambuf_iterator<char>{}};

  // Parse each line
  auto oui = std::map<std::string, std::string>{
      {"f2-ed-07", "Nothing Technology Limited"},
  };

  for (auto line : str | std::views::split('\n')) {

    // Look for all the lines with the (hex) string
    auto s = std::string{line.begin(), line.end()};
    if (not s.contains("(hex)") or s.empty())
      continue;

    auto pos = s.find("(hex)");

    if (pos != std::string::npos) {
      auto key = s.substr(0, 8);
      auto value = s.substr(pos + 6);

      // Make the key lower case
      std::transform(key.begin(), key.end(), key.begin(), ::tolower);

      // Remove control characters in value
      value.erase(std::remove_if(value.begin(), value.end(),
                                 [](char c) { return std::iscntrl(c); }),
                  value.end());

      oui[key] = value;
    }
  }

  return oui;
}

// Initialise the database on startup
const auto database = get_oui();
} // namespace

// Lookup a MAC address in the database
namespace oui {
std::string lookup(const std::string_view mac) {

  auto key = mac.substr(0, 8);
  auto it = database.find(std::string{key});

  return it != database.end() ? it->second : "";
}
} // namespace oui