#include "oui.h"
#include "packet.h"
#include "types.h"
#include <cassert>
#include <chrono>
#include <format>
#include <iostream>
#include <map>
#include <mutex>
#include <ranges>
#include <string>
#include <syncstream>
#include <thread>
#include <vector>

int main() {
  using namespace std::chrono_literals;

  // Shared data structure for storing captured packets
  auto packet_mutex = std::mutex{};
  auto packets = std::vector<ethernet_packet_t>{};

  {
    // Thread pool
    auto threads = std::vector<std::jthread>{};

    // Get all network interfaces
    auto interfaces = cap::interfaces();

    // If there's an "any" interface, just use that
    constexpr auto catch_all = "any";
    if (interfaces.contains(catch_all))
      interfaces = {catch_all};

    assert(not std::empty(interfaces));

    // Start a thread to capture on each interface
    for (auto interface : interfaces) {
      threads.emplace_back(
          [&](std::stop_token token, std::string interface) {
            // Create capture object
            auto capture = cap::packet_t{interface};

            // Capture until stop is requested
            while (not token.stop_requested()) {

              // Read a packet
              auto packet = capture.read();

              // And store it if valid
              std::scoped_lock lock{packet_mutex};
              if (not std::empty(packet.source_.mac_))
                packets.push_back(packet);
            }
          },
          interface);
    }

    // Start thread to process packets
    threads.emplace_back([&packets, &packet_mutex](std::stop_token token) {
      std::map<std::string, ethernet_packet_t> devices;

      while (not token.stop_requested()) {

        // Sleep for a while
        std::this_thread::sleep_for(1s);

        std::osyncstream{std::cout}
            << std::format("\nPackets per second: {}\n", std::size(packets));

        // Process the packets
        {
          std::scoped_lock lock{packet_mutex};
          for (auto &packet : packets)
            devices.emplace(packet.source_.mac_, packet);

          // Clear down the packets
          packets.clear();
        }

        // Print the devices
        for (auto &[mac, device] : devices)
          std::osyncstream{std::cout}
              << std::format("{:15} {} {}\n", device.source_.ip_,
                             oui::lookup(mac), device.info);
      }
    });

    // Capture packets for a while
    std::this_thread::sleep_for(10s);

    // Stop all the threads
    for (auto &thread : threads)
      thread.request_stop();
  }

  std::osyncstream{std::cout} << "\nGoodnight\n";
}