#include "packet.h"
#include "types.h"
#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <format>
#include <ranges>

namespace cap {

// Only constructor allowed
packet_t::packet_t(std::string name) {

  interface = name;

  // Capture arguments
  char errbuf[256];
  auto snaplen = 65535;
  auto promiscuous = 1;
  auto timeout_ms = 10;

  // Open the device
  pcap = pcap_open_live(name.c_str(), snaplen, promiscuous, timeout_ms, errbuf);

  // Set non-blocking mode
  if (pcap != nullptr)
    pcap_setnonblock(pcap, 1, errbuf);
}

// RAII destructor
packet_t::~packet_t() {
  if (pcap != nullptr) {
    pcap_close(pcap);
    pcap = nullptr;
  }
}

// Read a single packet from the interface
ethernet_packet_t packet_t::read() {

  if (pcap == nullptr)
    return {};

  pcap_pkthdr header;
  const u_char *data = pcap_next(pcap, &header);

  if (data == nullptr)
    return {};

  // Structure of the first part of the packet
  struct ethernet_header_t {
    uint8_t destination_mac[6]{};
    uint8_t source_mac[6]{};
    uint16_t packet_type{};
  };

  static_assert(sizeof(ethernet_header_t) == 14);
  assert(header.len >= sizeof(ethernet_header_t));

  auto eth = *reinterpret_cast<const ethernet_header_t *>(data);

  auto source_mac =
      std::format("{:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}",
                  eth.source_mac[0], eth.source_mac[1], eth.source_mac[2],
                  eth.source_mac[3], eth.source_mac[4], eth.source_mac[5]);

  auto destination_mac = std::format(
      "{:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}", eth.destination_mac[0],
      eth.destination_mac[1], eth.destination_mac[2], eth.destination_mac[3],
      eth.destination_mac[4], eth.destination_mac[5]);

  auto source_ip = std::string{};
  auto destination_ip = std::string{};

  // Get the IPs if it's an IPv4 packet
  if (eth.packet_type == 0x0008) {

    // Map the IPv4 structure onto these data
    auto ip = *reinterpret_cast<const ip_header_t *>(data +
                                                     sizeof(ethernet_header_t));

    // Extract from IP
    source_ip = std::format("{}.{}.{}.{}", ip.source_ip[0], ip.source_ip[1],
                            ip.source_ip[2], ip.source_ip[3]);

    // Extract to IP
    destination_ip = std::format("{}.{}.{}.{}", ip.dest_ip[0], ip.dest_ip[1],
                                 ip.dest_ip[2], ip.dest_ip[3]);
  }

  return {
      .interface = interface,
      .source = {.mac = source_mac, .ip = source_ip},
      .destination = {.mac = destination_mac, .ip = destination_ip},
      .type = eth.packet_type,
      .length = header.len,
  };
}

// List all network interfaces
std::vector<std::string> interfaces() {

  auto names = std::vector<std::string>{};

  // Find all network interfaces
  pcap_if_t *all_devices;
  char errbuf[256];
  if (pcap_findalldevs(&all_devices, errbuf) < 0)
    return names;

  // Extract all the good ones
  for (auto dev = all_devices; dev != nullptr; dev = dev->next) {

    // Exclude list
    auto name = std::string{dev->name};
    constexpr auto excludes = std::array{"any", "nflog", "gpd0"};

    // Skip the excluded interfaces
    if (not std::ranges::any_of(
            excludes, [name](auto excluded) { return name == excluded; }))
      names.emplace_back(name);
  }

  return names;
}

} // namespace cap
