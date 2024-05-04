#pragma once

#include <cstdint>
#include <string>

// Information gathered about devices seen on the network
struct device_t {
  size_t packets{};
  size_t packet_length{};
  uint16_t packet_type{};
  std::string ip{};
  std::string vendor{};
};

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