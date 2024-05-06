#pragma once

#include "packet.h"
#include "types.h"
#include <pcap.h>
#include <string>
#include <vector>

namespace cap {

// List all network interfaces
std::vector<std::string> interfaces();

// RAII wrapper for a capture interface
class packet {

  // Name of the network interface
  std::string interface_{};

  // Handle to the pcap library
  pcap_t *pcap_ = nullptr;

public:
  // Start capture on a single interface
  explicit packet(std::string_view);

  // Only explicit constructor allowed
  packet() noexcept = delete;
  packet(const packet &) noexcept = delete;
  packet(packet &&) noexcept = delete;
  packet &operator=(const packet &) noexcept = delete;
  packet &operator=(packet &&) noexcept = delete;

  // Read a single packet from the interface
  ethernet_packet_t read();

  // RAII destructor
  ~packet();
};

} // namespace cap