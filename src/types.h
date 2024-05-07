#pragma once

#include <cstdint>
#include <string>

// Shared data structure for devices seen on the network
struct ethernet_packet_t {

  // The interface the packet was captured on
  std::string interface_{};

  // Source and destination devices
  struct {
    std::string mac_{};
    std::string ip_{};
  } source_{}, destination_{};

  // Type of the packet, which indicates the payload structure
  uint16_t type_{};
};

// IPv4 structure
struct ip_header_t {
  uint8_t version_ihl_;
  uint8_t dscp_ecn_;
  uint16_t total_length_;
  uint16_t identification_;
  uint16_t flags_fragment_offset_;
  uint8_t ttl_;
  uint8_t protocol_;
  uint16_t checksum_;
  uint8_t source_ip_[4];
  uint8_t dest_ip_[4];
};

static_assert(sizeof(ip_header_t) == 20);
