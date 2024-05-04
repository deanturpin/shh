#include "capture.h"
#include "device.h"
#include <format>
#include <pcap.h>
#include <ranges>

namespace cap {

// Capture packets from a single network interface
std::multimap<std::string, device_t> read(std::string_view network_interface) {

  auto devices = std::multimap<std::string, device_t>{};

  // Open the network interface in promiscuous mode
  char errbuf[256];
  pcap_t *pcap = pcap_open_live(std::string{network_interface}.c_str(), 65535,
                                1, 1000, errbuf);

  if (pcap == nullptr)
    return devices;

  // Process number of packets
  for (auto _ : std::views::iota(0, 20)) {

    // Read packets
    pcap_pkthdr header;
    const u_char *data = pcap_next(pcap, &header);

    if (data == nullptr)
      continue;

    // Create a new device to describe this packet
    auto device_source = device_t{};
    auto device_dest = device_t{};

    struct ethernet_header {
      uint8_t destMac[6]; // Destination MAC address
      uint8_t srcMac[6];  // Source MAC address
      uint16_t etherType; // Ethernet type
    };

    auto eth_header = reinterpret_cast<ethernet_header const *const>(data);

    // Extract MAC addresses
    auto mac_source = std::format("{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}",
                                  eth_header->srcMac[0], eth_header->srcMac[1],
                                  eth_header->srcMac[2], eth_header->srcMac[3],
                                  eth_header->srcMac[4], eth_header->srcMac[5]);

    auto mac_dest = std::format("{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}",
                                eth_header->destMac[0], eth_header->destMac[1],
                                eth_header->destMac[2], eth_header->destMac[3],
                                eth_header->destMac[4], eth_header->destMac[5]);

    auto packet_type = eth_header->etherType;

    // Copy these data into the outgoing device
    device_source.packet_type = device_dest.packet_type = packet_type;
    device_source.packet_length = device_dest.packet_length = header.len;
    device_source.packets = device_dest.packets = 1;

    // Only set IP address if it's a suitable packet type
    if (packet_type == 0x0008) {
      device_source.ip =
          std::format("{}.{}.{}.{}", data[26], data[27], data[28], data[29]);
      device_dest.ip =
          std::format("{}.{}.{}.{}", data[30], data[31], data[32], data[33]);
    }

    // Add device to the list
    devices.emplace(mac_source, device_source);
    devices.emplace(mac_dest, device_dest);
  }

  return devices;
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
