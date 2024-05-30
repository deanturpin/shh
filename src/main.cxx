#include "oui.h"
#include "packet.h"
#include "types.h"
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <format>
#include <future>
#include <map>
#include <mutex>
#include <print>
#include <ranges>
#include <string>
#include <vector>

// The git hash is compiled into the binary
#ifndef GIT_HASH
#define GIT_HASH ""
#endif

int main() {

  using namespace std::chrono_literals;

  std::atexit([] { std::println("God natt"); });

  std::println("https://github.com/deanturpin/shh/commit/{}\n", GIT_HASH);

  // Shared data structure for storing captured packets
  auto packet_mutex = std::mutex{};
  auto packets = std::vector<ethernet_packet_t>{};

  // Get all network interfaces
  auto interfaces = cap::interfaces();
  assert(not std::empty(interfaces));

  // Hit the ground running
  std::atomic_bool running = true;

  // Container for the promises
  // These are counts of packets captured on each interface
  std::vector<std::future<size_t>> counts;

  // The capture routine
  auto func = [&](auto name) {
    // Return the number of packets captured
    auto total_packets = 0uz;

    // Create capture object
    auto capture = cap::packet_t{name};

    // Capture packets until told to stop
    while (running) {
      ethernet_packet_t packet = capture.read();

      // If it's not valid, catch your breath and skip it
      if (packet.source.mac.empty()) {
        std::this_thread::sleep_for(1ms);
        continue;
      }

      ++total_packets;

      // Otherwise store the packet
      std::scoped_lock lock{packet_mutex};
      packets.push_back(packet);
    }

    return total_packets;
  };

  // Start all the threads
  for (auto name : interfaces)
    counts.emplace_back(std::async(std::launch::async, func, name));

  // Duration of logging, application will exit after this time
  constexpr auto logging_cycles = 60uz;

  for (auto i : std::views::iota(0uz, logging_cycles)) {

    // Sleep for a while
    auto interval = 1000ms;
    std::this_thread::sleep_for(interval);

    static std::map<std::string, ethernet_packet_t> devices;

    auto total_bytes = 0uz;
    auto total_packets = 0uz;

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

    std::println("\n{} packets @ {:.3f} Mb/s - {}/{}\n", total_packets,
                 (total_bytes * 8 / 1'000'000.0) / interval.count(), i + 1,
                 logging_cycles);
  }

  std::println("Stopping {} threads", interfaces.size());

  // Ask all the threads to finish what they're doing
  running = false;

  // Wrap names and return values for convenience
  auto zipped = std::views::zip(interfaces, counts);

  // Wait for all the threads to finish
  for (const auto &[name, count] : zipped)
    std::println("{:4} {:16}", count.get(), name);
}
