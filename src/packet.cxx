#include "packet.h"
#include "types.h"
#include <bit>
#include <cassert>
#include <format>

namespace cap {

// Only constructor allowed
packet_t::packet_t(std::string_view interface) {

  interface = interface;

  // Capture arguments
  char errbuf[256];
  auto ni = std::string{interface}.c_str();
  auto snaplen = 65535;
  auto promiscuous = 1;
  auto timeout_ms = 1000;

  // Open the device
  pcap_ = pcap_open_live(ni, snaplen, promiscuous, timeout_ms, errbuf);
}

// RAII destructor
packet_t::~packet_t() {
  if (pcap_ != nullptr) {
    pcap_close(pcap_);
    pcap_ = nullptr;
  }
}

// Read a single packet from the interface
ethernet_packet_t packet_t::read() {
  u_char const *data;
  pcap_pkthdr *header;
  auto success = pcap_next_ex(pcap_, &header, &data);

  if (success != 1)
    return {};

  // Structure of the first part of the packet
  struct ethernet_header_t {
    uint8_t destination_mac[6]{};
    uint8_t source_mac[6]{};
    uint16_t packet_type{};
  };

  assert(header->len >= sizeof(ethernet_header_t));
  static_assert(sizeof(ethernet_header_t) == 14);

  // Map these data into the header structure
  auto eth =
      std::span<ethernet_header_t>(
          reinterpret_cast<ethernet_header_t *>(const_cast<u_char *>(data)), 1)
          .front();

  // Extract the MAC addresses
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
  if (std::byteswap(eth.packet_type) == 0x0800) {

    // Map the IPv4 structure onto these data
    auto ip =
        reinterpret_cast<const ip_header_t *>(data + sizeof(ethernet_header_t));

    // Extract from IP
    source_ip = std::format("{}.{}.{}.{}", ip->source_ip[0], ip->source_ip[1],
                            ip->source_ip[2], ip->source_ip[3]);

    // Extract to IP
    destination_ip =
        std::format("{}.{}.{}.{}", ip->dest_ip[0], ip->dest_ip[1],
                    ip->dest_ip[2], ip->dest_ip[3]);
  }

  return {
      .interface = interface,
      .info = std::string{},
      .source = {.mac = source_mac, .ip = source_ip},
      .destination = {.mac = destination_mac, .ip = destination_ip},
      .type = std::byteswap(eth.packet_type),
      .length = header->len,
  };
}

// Make assertions about the class
static_assert(not std::is_default_constructible_v<packet_t>);
static_assert(not std::is_copy_constructible_v<packet_t>);
static_assert(not std::is_copy_assignable_v<packet_t>);
static_assert(not std::is_move_constructible_v<packet_t>);
static_assert(not std::is_move_assignable_v<packet_t>);
static_assert(std::is_destructible_v<packet_t>);
static_assert(std::is_constructible_v<packet_t, std::string_view>);
static_assert(not std::has_virtual_destructor_v<packet_t>);

// List all network interfaces
std::set<std::string> interfaces() {

  std::set<std::string> network_interfaces{};

  // Find all network interfaces
  pcap_if_t *alldevs;
  char errbuf[256];
  if (pcap_findalldevs(&alldevs, errbuf) >= 0)
    for (pcap_if_t *d = alldevs; d != nullptr; d = d->next)
      network_interfaces.emplace(d->name);

  return network_interfaces;
}

} // namespace cap
