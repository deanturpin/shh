#pragma once

#include <cstdint>
#include <string>

// Shared data structure for devices seen on the network
struct ethernet_packet_t {

  // The interface the packet was captured on
  std::string interface_{};

  // Some interesting data from the packet
  std::string info{};

  // Source and destination devices
  struct {
    std::string mac{};
    std::string ip{};
  } source{}, destination{};

  // Type of the packet, which indicates the payload structure
  uint16_t type_{};
  size_t length{};
};

// IPv4 structure
struct ip_header_t {
  uint8_t version_ihl;
  uint8_t dscp_ecn;
  uint16_t total_length_;
  uint16_t identification;
  uint16_t flags_fragment_offset;
  uint8_t ttl;
  uint8_t protocol;
  uint16_t checksum;
  uint8_t source_ip[4];
  uint8_t dest_ip[4];
};

static_assert(sizeof(ip_header_t) == 20);
