#pragma once

#include "packet.h"
#include "types.h"
#include <pcap.h>
#include <set>
#include <string>
#include <vector>

namespace cap {

// List all network interfaces
std::set<std::string> interfaces();

// RAII wrapper for a capture interface
class packet_t {

  // Name of the network interface
  std::string interface{};

  // Handle to the pcap library
  pcap_t *pcap_ = nullptr;

public:
  // Start capture on a single interface
  explicit packet_t(std::string_view);

  // Only explicit constructor allowed
  packet_t() noexcept = delete;
  packet_t(const packet_t &) noexcept = delete;
  packet_t(packet_t &&) noexcept = delete;
  packet_t &operator=(const packet_t &) noexcept = delete;
  packet_t &operator=(packet_t &&) noexcept = delete;

  // Read a single packet from the interface
  ethernet_packet_t read();

  // RAII destructor
  ~packet_t();
};

} // namespace cap