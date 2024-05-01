#include <array>
#include <cassert>
#include <fstream>
#include <map>
#include <pcap.h>
#include <print>
#include <ranges>
#include <string.h>
#include <string>
#include <string_view>

constexpr std::array quotes{
    "Hello? Is it me you're looking for?",
    "It's oh so quiet",
    "Ssh...",
    "Hush now",
    "Silence is golden",
    "Hello, Dave?",
    "It hertz when IP",
    "Suddenly IP",
};

// Get a random quote
constexpr std::string_view get_quote() {
  // Get random index
  auto index = std::rand() % quotes.size();
  assert(index < quotes.size());

  // Return a random quote
  return quotes[std::rand() % quotes.size()];
}

// Capture packets from the given network device
void capture(std::string_view network_device) {

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
  for (auto _ : std::views::iota(0, 100)) {

    // Read packets
    pcap_pkthdr header;
    const u_char *data = pcap_next(pcap, &header);

    if (data == nullptr) {
      std::println("{}", get_quote());
      return;
    }

    // Print device name
    std::print("{}\t", network_device);

    // print packet data
    for (auto i = size_t{}; i < header.len; ++i) {

      // Print the mac address
      if (i < 6) {
        std::print("{:02x}", data[i]);

        if (i < 5)
          std::print(":");
      } else
        break;
    }

    // Print packet type
    std::print("\t- {:02x}{:02x}", data[12], data[13]);

    // Print ip addresses
    std::print(" - {}.{}.{}.{}", data[26], data[27], data[28],
               data[29]);

    // Print packet length
    std::println("\t- {} bytes", header.len);
  }
}

int main() {
  std::println("Careless Wispa");

  // List network devices
  std::println("Network devices:");

  pcap_if_t *alldevs;
  char errbuf[256];

  if (pcap_findalldevs(&alldevs, errbuf) == -1) {
    std::println("Error in pcap_findalldevs: {}", errbuf);
    return 1;
  }

  for (pcap_if_t *d = alldevs; d != nullptr; d = d->next)
    std::println("\t{}", d->name);

  // Capture a batch of packets from each network device
  for (pcap_if_t *d = alldevs; d != nullptr; d = d->next) {
    capture(d->name);
    break;
  }

  std::println("cya!");
}