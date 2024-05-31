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
class packet_t {

  // Name of the network interface
  std::string interface{};

  // Handle to the pcap library
  pcap_t *pcap = nullptr;

public:
  // Start capture on a single interface
  explicit packet_t(std::string);

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

// Make assertions about the class
static_assert(not std::is_default_constructible_v<packet_t>);
static_assert(not std::is_copy_constructible_v<packet_t>);
static_assert(not std::is_copy_assignable_v<packet_t>);
static_assert(not std::is_move_constructible_v<packet_t>);
static_assert(not std::is_move_assignable_v<packet_t>);
static_assert(std::is_destructible_v<packet_t>);
static_assert(std::is_constructible_v<packet_t, std::string>);
static_assert(not std::has_virtual_destructor_v<packet_t>);

} // namespace cap