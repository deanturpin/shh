#include "oui.h"
#include "packet.h"
#include "types.h"
#include <algorithm>
#include <atomic>
#include <cassert>
#include <format>
#include <future>
#include <iostream>
#include <mutex>
#include <string>
#include <syncstream>

int main() {
  using namespace std::chrono_literals;

  // Control the threads
  std::atomic_bool run{true};

  // Stop after this many packets
  constexpr auto max_packets = 50;

  // Shared data structure for packets
  auto packets_mutex = std::mutex{};
  auto packets = std::vector<ethernet_packet_t>{};
  packets.reserve(max_packets);

  // Start capture progress thread
  auto finished = std::async(std::launch::async, [&] {
    while (run) {
      std::osyncstream{std::cout} << std::format(
          "Packets received: {}/{}\n", std::size(packets), max_packets);
      std::this_thread::sleep_for(1s);
    }

    return true;
  });

  // Get all network interfaces
  auto network_interfaces = cap::interfaces();

  // If there's an "any" interface, just use that
  if (std::find(std::begin(network_interfaces), std::end(network_interfaces),
                "any") != std::end(network_interfaces))
    network_interfaces = {"any"};

  std::for_each(
// I cannot believe this isn't available for macOS clang 19
#ifdef notnowthanks
      std::execution::sequenced_policy,
#endif
      std::begin(network_interfaces), std::end(network_interfaces),
      [&](auto &interface) {
        // Create capture object
        auto cap = cap::packet_t{interface};

        // Read one packet at a time until the buffer is full
        while (run) {
          auto pac = cap.read();

          // Check if the packet is empty
          if (not std::empty(pac.source_.mac_)) {

            std::scoped_lock lock{packets_mutex};

            if (std::size(packets) < max_packets) {
              packets.push_back(pac);
            } else
              run = false;
          }
        }
      });

  // Wait for the reporting thread to finish
  auto confirm = finished.get();
  assert(confirm);

  // Print the packets
  std::osyncstream{std::cout}
      << std::format("Packets received: {}\n", std::size(packets));

  for (auto &packet : packets) {

    // Resolve the vendors or just print the MAC address
    auto source_vendor = oui::lookup(packet.source_.mac_);
    auto dest_vendor = oui::lookup(packet.destination.mac_);

    std::osyncstream{std::cout} << std::format(
        "{:6} {:04x} {} > {}\n", packet.interface_, packet.type,
        std::empty(source_vendor) ? packet.source_.mac_ : source_vendor,
        std::empty(dest_vendor) ? packet.destination.mac_ : dest_vendor);
  }

  std::osyncstream{std::cout} << std::format("goodnight\n");
}