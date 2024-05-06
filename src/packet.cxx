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

  struct ethernet_header {
    uint8_t destMac[6]; // Destination MAC address
    uint8_t srcMac[6];  // Source MAC address
    uint16_t etherType; // Ethernet type
  };

  // Map these data into the header structure
  auto eth = reinterpret_cast<const ethernet_header *>(data);

  // Extract the MAC addresses
  auto mac_source = std::format("{:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}",
                                eth->srcMac[0], eth->srcMac[1], eth->srcMac[2],
                                eth->srcMac[3], eth->srcMac[4], eth->srcMac[5]);

  auto mac_dest =
      std::format("{:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}", eth->destMac[0],
                  eth->destMac[1], eth->destMac[2], eth->destMac[3],
                  eth->destMac[4], eth->destMac[5]);

  return {
      .interface_ = interface_,
      .source = {.mac = mac_source},
      .destination = {.mac = mac_dest},
      .type = eth->etherType,
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
