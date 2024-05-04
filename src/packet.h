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

  std::string interface_{};
  pcap_t *pcap = nullptr;

public:
  // Only explicit constructor allowed
  packet() noexcept = delete;
  packet(const packet &) noexcept = delete;
  packet(packet &&) noexcept = delete;
  packet &operator=(const packet &) noexcept = delete;
  packet &operator=(packet &&) noexcept = delete;

  // Read a single packet from the interface
  ethernet_packet_t read();

  // Start capture on a single interface
  explicit packet(std::string_view interface) {

    interface_ = interface;

    char errbuf[256];
    pcap =
        pcap_open_live(std::string{interface}.c_str(), 65535, 1, 1000, errbuf);
  }

  // RAII destructor
  ~packet();
};

} // namespace cap