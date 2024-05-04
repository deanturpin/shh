#include "capture.h"
#include "device.h"
#include <format>
#include <pcap.h>
#include <ranges>

namespace cap {

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
