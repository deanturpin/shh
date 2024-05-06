#pragma once

#include <cstdint>
#include <string>

// Shared data structure for devices seen on the network
// See: dependency inversion principle
struct ethernet_packet_t {

  // The interface the packet was captured on
  std::string interface_{};

  // Source and destination devices
  struct {
    std::string mac_{};
    std::string ip_{};
  } source_{}, destination_{};

  // Type of the packet: TCP, UDP, ICMP
  uint16_t type_{};
};