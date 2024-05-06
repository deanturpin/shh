#include "packet.h"
#include "types.h"
#include <cassert>
#include <format>

namespace cap {

// Only constructor allowed
packet_t::packet_t(std::string_view interface) {

  interface_ = interface;

  char errbuf[256];
  pcap_ =
      pcap_open_live(std::string{interface}.c_str(), 65535, 1, 1000, errbuf);
}

// RAII destructor
packet_t::~packet_t() {
  if (pcap_ != nullptr)
    pcap_close(pcap_);
}

// Read a single packet from the interface
ethernet_packet_t packet_t::read() {
  pcap_pkthdr header;
  const u_char *data = pcap_next(pcap_, &header);

  if (data == nullptr)
    return {};

  // Structure of the first part of the packet
  struct ethernet_header_t {
    uint8_t destination_mac_[6];
    uint8_t source_mac_[6];
    uint16_t packet_type_;
  };

  // Map these data into the header structure
  auto eth = reinterpret_cast<const ethernet_header_t *>(data);

  // Extract the MAC addresses
  auto mac_source = std::format("{:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}",
                                eth->source_mac_[0], eth->source_mac_[1],
                                eth->source_mac_[2], eth->source_mac_[3],
                                eth->source_mac_[4], eth->source_mac_[5]);

  auto mac_dest =
      std::format("{:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}",
                  eth->destination_mac_[0], eth->destination_mac_[1],
                  eth->destination_mac_[2], eth->destination_mac_[3],
                  eth->destination_mac_[4], eth->destination_mac_[5]);

  return {
      .interface_ = interface_,
      .source_ = {.mac_ = mac_source},
      .destination = {.mac_ = mac_dest},
      .type_ = eth->packet_type_,
  };
}

// List all network interfaces
std::vector<std::string> interfaces() {
  pcap_if_t *alldevs;
  char errbuf[256];

  // Create container for network interfaces
  std::vector<std::string> network_interfaces{};

  // Find all network interfaces
  if (pcap_findalldevs(&alldevs, errbuf) == -1)
    return network_interfaces;

  // Convert to strings
  for (pcap_if_t *d = alldevs; d != nullptr; d = d->next)
    network_interfaces.push_back(d->name);

  return network_interfaces;
}

} // namespace cap
