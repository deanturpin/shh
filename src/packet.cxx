#include "packet.h"
#include "types.h"
#include <cassert>
#include <format>

namespace cap {

// RAII destructor
packet::~packet() {
  if (pcap != nullptr)
    pcap_close(pcap);
}

// Read a single packet from the interface
ethernet_packet_t packet::read() {

  pcap_pkthdr header;
  const u_char *data = pcap_next(pcap, &header);

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
      .source = {.mac = mac_source},
      .destination = {.mac = mac_dest},
      .type = eth->packet_type_,
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
