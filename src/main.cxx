#include <array>
#include <cassert>
#include <pcap.h>
#include <print>
#include <ranges>
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
};

constexpr auto get_quote() {
  // Get random index
  auto index = std::rand() % quotes.size();
  assert(index < quotes.size());

  // Return a random quote
  return quotes[std::rand() % quotes.size()];
}

void capture(std::string_view network_device) {

  char errbuf[256];

  //   constexpr auto network_device = +"en0";
  std::println(stderr, "Opening network device: '{}'", network_device);

  // Open the network device in promiscuous mode
  pcap_t *pcap = pcap_open_live(std::string{network_device}.c_str(), 65535, 1,
                                1000, errbuf);

  if (pcap == nullptr) {
    std::println("Error opening network device: '{}'",
                 std::string_view(errbuf));
    return;
  }

  for (auto _ : std::views::iota(0, 200)) {

    // Read packets
    pcap_pkthdr header;
    const u_char *data = pcap_next(pcap, &header);

    if (data == nullptr) {
      std::println(stderr, "{}", get_quote());
      return;
    }

    // Print device name
    std::print(stderr, "{}\t", network_device);

    // print packet data
    for (auto i = size_t{}; i < header.len; ++i) {

      // Print the mac address
      if (i < 6) {
        std::print(stderr, "{:02x}", data[i]);

        if (i < 5)
          std::print(":");
      } else
        break;
    }

    // print packet type
    std::print(stderr, "\t- {:02x}{:02x}", data[12], data[13]);

    // print ip addresses
    std::print(stderr, " - {}.{}.{}.{}", data[26], data[27], data[28],
               data[29]);

    // print packet length
    std::println(stderr, "\t- {} bytes", header.len);
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

  for (pcap_if_t *d = alldevs; d != nullptr; d = d->next) {

    std::println("\t{}", d->name);
    capture(d->name);
    break;
  }

  std::println("cya!");
}