#include <algorithm>
#include <fstream>
#include <map>
#include <ranges>
#include <string>

std::map<std::string, std::string> get_oui() {

  auto in = std::ifstream{"oui.txt"};

  // Read whole file into a string
  auto str = std::string{std::istreambuf_iterator<char>{in},
                         std::istreambuf_iterator<char>{}};

  // Parse each line
  auto oui = std::map<std::string, std::string>{};

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