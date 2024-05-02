#pragma once

#include <cstdint>
#include <string>

// Information about network devices

struct device_t {
  size_t packets{};
  size_t packet_length{};
  uint16_t packet_type{};
  std::string ip{};
  std::string vendor{};
};