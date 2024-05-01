#include <array>
#include <atomic>
#include <cassert>
#include <chrono>
#include <fstream>
#include <map>
#include <pcap.h>
#include <print>
#include <ranges>
#include <string.h>
#include <string>
#include <string_view>
#include <thread>

constexpr std::array quotes{
    "Hello? Is it me you're looking for?",
    "It's oh so quiet",
    "Ssh...",
    "Hush now",
    "Silence is golden",
    "Hello, Dave?",
    "It hertz when IP",
    "Everybody hertz",
    "Suddenly IP",
};

auto get_oui() {

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

      //   std::println("{} -> {}", key, value);
    }
  }

  return oui;
}

// Get a random quote
constexpr std::string_view get_quote() {
  // Get random index
  auto index = std::rand() % quotes.size();
  assert(index < quotes.size());

  // Return a random quote
  return quotes[std::rand() % quotes.size()];
}

// Capture packets from the given network device
void capture(std::string_view network_device,
             std::map<std::string, std::string> &oui) {

  char errbuf[256];
  std::println("Opening network device: '{}'", network_device);

  // Open the network device in promiscuous mode
  pcap_t *pcap = pcap_open_live(std::string{network_device}.c_str(), 65535, 1,
                                1000, errbuf);

  if (pcap == nullptr) {
    std::println("Error opening network device: '{}'",
                 std::string_view(errbuf));
    return;
  }

  // Process number of packets
  for (auto _ : std::views::iota(0, 1000)) {

    // Read packets
    pcap_pkthdr header;
    const u_char *data = pcap_next(pcap, &header);

    if (data == nullptr) {
      std::println("{}", get_quote());
      continue;
    }

    // Print device name
    std::print("{} ", network_device);

    // struct EthernetHeader {
    //     uint8_t destMac[6];  // Destination MAC address
    //     uint8_t srcMac[6];   // Source MAC address
    //     uint16_t etherType;  // Ethernet type
    // };

    // Extract MAC address
    auto mac = std::string{};
    for (auto i = size_t{6}; i < 12; ++i)
      mac += std::format("{:02x}", data[i]) + "-";

    // Extract vendor from mac
    auto short_vendor = mac.substr(0, 8);

    // Check if vendor is in OUI
    auto vendor = oui.contains(short_vendor) ? oui[short_vendor]
                                             : short_vendor + " unknown";

    // Print packet type
    std::print("{:02x}{:02x} ", data[12], data[13]);

    // Print source ip address
    std::print("{}.{}.{}.{} > ", data[30], data[31], data[32], data[33]);

    // Print destination ip address
    std::print("{}.{}.{}.{} ", data[26], data[27], data[28], data[29]);

    // print vendor
    std::print("{} ", vendor);

    // Print packet length
    std::println("({} bytes)", header.len);
  }
}

int main() {

  //   std::print("Processing vendor file... ");
  //   auto oui = get_oui();
  //   std::println("{} vendors", oui.size());

  //   // print first few vendors
  //   for (auto [key, value] : oui | std::views::take(20))
  //     std::println("{} -> {} ({} {})", key, value, key.size(), value.size());

  //   // List network devices
  //   std::println("Network devices:");

  //   pcap_if_t *alldevs;
  //   char errbuf[256];

  //   if (pcap_findalldevs(&alldevs, errbuf) == -1) {
  //     std::println("Error in pcap_findalldevs: {}", errbuf);
  //     return 1;
  //   }

  //   for (pcap_if_t *d = alldevs; d != nullptr; d = d->next)
  //     std::println("\t{}", d->name);

  //   // Capture a batch of packets from each network device
  //   for (pcap_if_t *d = alldevs; d != nullptr; d = d->next) {
  //     capture(d->name, oui);
  //     break;
  //   }

  //   std::println("Freeing network devices");
  //   pcap_freealldevs(alldevs);

  //   std::println("Create thread for sniffing");

  // Control the threads
  std::atomic_bool stop{false};

  // Shared MAC data
  std::mutex mac_mutex;
  std::map<std::string, size_t> macs;

  // Search for MAC addresses
  std::thread sniffer{[&]() {
    while (not stop.load()) {

      // std::println("Sniffing...");
      {
        std::scoped_lock lock{mac_mutex};
        ++macs["00:00:00:00:00:00"];
      }

      std::this_thread::sleep_for(std::chrono::microseconds(1));
    }

    std::println("Sniffer stopped");
  }};

  // Report MACs seen and packet count
  std::thread reporter{[&]() {
    while (not stop) {

      // Clear terminal
      std::print("\033[2J\033[1;1H");

      {
        std::scoped_lock lock{mac_mutex};
        for (auto [mac, count] : macs)
          std::println("{} {}", mac, count);
      }

      std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::println("Reporter stopped");
  }};

  //   // Wait for ten seconds
  std::this_thread::sleep_for(std::chrono::seconds(10));

  // Request stop
  stop.store(true);

  // Wait for the threads to finish
  if (sniffer.joinable())
    sniffer.join();

  if (reporter.joinable())
    reporter.join();

  std::println("cya!");
}