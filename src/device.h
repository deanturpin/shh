#pragma once

#include <cstdint>
#include <string>

// Shared data structure for devices seen on the network
struct packet_t {

  // The interface the packet was captured on
  std::string interface{};

  // Source and destination devices
  struct {
    std::string mac{};
    std::string ip{};
  } source, destination;

  // Type of the packet: TCP, UDP, ICMP
  uint16_t type{};
};