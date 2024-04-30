#include <array>
#include <cassert>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <pcap.h>
#include <print>
#include <string>
#include <string_view>
// #include <future>
#include <ranges>
#include <thread>

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

static_assert(not std::empty(quotes));

// // file header
// struct pcap_file_header {
//   std::uint32_t magic_number;  // Magic number
//   std::uint16_t version_major; // Major version number
//   std::uint16_t version_minor; // Minor version number
//   std::int32_t thiszone;       // GMT to local correction
//   std::uint32_t sigfigs;       // Accuracy of timestamps
//   std::uint32_t snaplen;       // Max length of captured packets
//   std::uint32_t network;       // Data link type
// };

// // packet header
// struct pcap_packet_header {
//   std::uint32_t ts_sec;   // Timestamp seconds
//   std::uint32_t ts_usec;  // Timestamp microseconds
//   std::uint32_t incl_len; // Number of octets of packet saved in file
//   std::uint32_t orig_len; // Actual length of packet
// };

// std::atexit([]() {
//   std::println("cya!");
//   return 0;
// });

void capture(std::string_view network_device) {

  char errbuf[256];

  //   constexpr auto network_device = +"en0";
  std::println("Opening network device: '{}'", network_device);

  // Open the network device in promiscuous mode
  pcap_t *pcap = pcap_open_live(std::string{network_device}.c_str(), 65535, 1,
                                1000, errbuf);

  if (pcap == nullptr) {
    std::println("Error opening network device: '{}'",
                 std::string_view(errbuf));
    return;
  }

  for (auto _ : std::views::iota(0, 10)) {

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
        if (i < 5) {
          std::print(":");
        }
      } else {
        break;
      }
    }

    // print ip addresses
    std::print(" - {}.{}.{}.{}", data[26], data[27], data[28], data[29]);

    // print packet length
    std::println("\t- {} bytes", header.len);
  }
  // // Set a filter (optional)
  // struct bpf_program filter;
  // pcap_compile(pcapHandle, &filter, "tcp port 80", 0, PCAP_NETMASK_UNKNOWN);
  // pcap_setfilter(pcapHandsle, &filter);

  // // Start capturing packets
  // std::cout << "Capturing packets..." << std::endl;
  // pcap_loop(pcapHandle, 0, packetHandler, nullptr);

  // // Close the pcap handle when done
  // pcap_close(pcapHandle);
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

//   while (true)
    for (pcap_if_t *d = alldevs; d != nullptr; d = d->next) {

      std::println("\t{}", d->name);
      capture(d->name);

      // std::jthread(std::launch::async, capture, d->name);
    }
}