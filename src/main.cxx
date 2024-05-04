#include "capture.h"
#include "oui.h"
#include "packet2.h"
#include <algorithm>
#include <atomic>
#include <future>
#include <mutex>
#include <print>
#include <string>
#include <thread>

int main() {
  using namespace std::chrono_literals;

  // Control the threads
  std::atomic_bool run{true};

  // Get all network interfaces
  auto network_interfaces = cap::interfaces();

  // Stop after this many packets
  constexpr auto max_packets = 10;

  // Shared data structure for packets
  auto packets_mutex = std::mutex{};
  auto packets = std::vector<packet_t>{};
  packets.reserve(max_packets);

  // Start capture progress thread
  auto finished = std::async(std::launch::async, [&] {
    while (run) {
      std::println("Packets received: {}/{}", std::size(packets), max_packets);
      std::this_thread::sleep_for(1s);
    }

    return true;
  });

  // Create container of threads
  auto threads = std::vector<std::thread>{};

  // Start a thread for each network interface
  for (auto &interface : network_interfaces) {
    threads.emplace_back([&] {
      {
        // Create capture object
        auto cap = packet{interface};

        while (run) {
          // Read one packet
          auto packet = cap.read();

          // Check if the packet is empty
          if (not std::empty(packet.source.mac)) {

            if (std::size(packets) < max_packets) {
              std::scoped_lock lock{packets_mutex};
              packets.push_back(packet);
            } else
              run = false;
          }
        }
      }

      std::println("Thread stopping for {}", interface);
    });
  }

  // Wait for the progress thread
  finished.get();

  for (auto &thread : threads)
    if (thread.joinable())
      thread.join();

  // Print the packets
  std::println("Packets received: {}", std::size(packets));
  for (auto &packet : packets) {

    // Resolve the vendors or just print the MAC address
    auto source_vendor = oui::lookup(packet.source.mac);
    auto dest_vendor = oui::lookup(packet.destination.mac);

    std::println("{:6} {:04x} {} > {}", packet.interface, packet.type,
                 std::empty(source_vendor) ? packet.source.mac : source_vendor,
                 std::empty(dest_vendor) ? packet.destination.mac
                                         : dest_vendor);
  }

  std::println("goodnight");
}