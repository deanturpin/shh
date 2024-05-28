#include "oui.h"
#include "packet.h"
#include "types.h"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <execution>
#include <format>
#include <map>
#include <mutex>
#include <print>
#include <stop_token>
#include <string>
#include <thread>
#include <vector>

#ifndef GIT_HASH
#define GIT_HASH "unknown"
#endif

int main() {

  using namespace std::chrono_literals;

  std::println("https://gitlab.com/deanturpin/shh @ {}\n", GIT_HASH);

  // Shared data structure for storing captured packets
  auto packet_mutex = std::mutex{};
  auto packets = std::vector<ethernet_packet_t>{};

  // Thread pool
  auto threads = std::vector<std::jthread>{};

  // Get all network interfaces
  auto interfaces = cap::interfaces();
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
            if (not std::empty(packet.source.mac))
              packets.push_back(packet);
          }

          std::println("Stopped capturing on {}\n", interface);
        },
        interface);
  }

  // Start thread to process packets
  threads.emplace_back([&packets, &packet_mutex](std::stop_token token) {
    std::map<std::string, ethernet_packet_t> devices;

    while (not token.stop_requested()) {

      // Sleep for a while
      auto interval = 1000ms;
      std::this_thread::sleep_for(interval);

      auto total_bytes = size_t{};
      auto total_packets = size_t{};
      static auto iterations = size_t{};

      // Process the packets
      {
        std::scoped_lock lock{packet_mutex};
        for (auto &packet : packets) {

          // Store packet size
          total_bytes += packet.length;

          // Store MAC addresses
          devices.emplace(packet.source.mac, packet);
          devices.emplace(packet.destination.mac, packet);
        }

        // Clear down the packets
        total_packets = packets.size();
        packets.clear();
      }

      // Print the devices
      for (auto &[mac, device] : devices)
        if (!device.source.ip.empty() || !oui::lookup(mac).empty())
          std::print("{:16} {:15} {:17} {:04x} {}\n", device.interface,
                     device.source.ip, mac, device.type, oui::lookup(mac));

      std::println("\n{} packets @ {:.3f} Mb/s ({})\n", total_packets,
                   (total_bytes * 8 / 1'000'000.0) / interval.count(),
                   iterations);

      ++iterations;
    }
  });

  // Capture packets for a while
  std::this_thread::sleep_for(10s);

  std::println("Stopping {} threads", threads.size());

  // Stop all the threads
  std::for_each(std::execution::par, threads.begin(), threads.end(),
                [](std::jthread &thread) {
                  thread.request_stop();
                  if (thread.joinable())
                    thread.join();
                });

  std::println("God natt");
}
