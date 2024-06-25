#include "oui.h"
#include "packet.h"
#include "types.h"
#include <barrier>
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

  // Exit through the git shop
  std::atexit([] {
    std::println("\nGod natt");
    std::println("https://github.com/deanturpin/shh/commit/{}\n", GIT_HASH);
  });

  // Shared data structure for storing captured packets
  auto packet_mutex = std::mutex{};
  auto packets = std::vector<ethernet_packet_t>{};

  // Get all network interfaces
  auto interfaces = cap::interfaces();
  assert(not std::empty(interfaces));

  // Hit the ground running
  std::atomic<bool> running = true;

  // Container for the promises
  // These are counts of packets captured on each interface
  std::vector<std::future<size_t>> counts;

  // Create a barrier to synchronize all threads
  std::barrier barrier(interfaces.size());

  // The capture routine
  auto func = [&](auto name) {
    // Return the number of packets captured
    auto total_packets = 0uz;

    // Create capture object
    auto capture = cap::packet_t{name};

    // Capture packets until told to stop
    while (running) {

      // Wait at the barrier until all threads are ready
      barrier.arrive_and_wait();

      // Read one packet
      ethernet_packet_t packet = capture.read();

      // If it's invalid, yield and skip it
      if (packet.source.mac.empty()) {
        std::this_thread::sleep_for(1ms);
        continue;
      }

      ++total_packets;

      // Otherwise store the packet
      std::lock_guard lock{packet_mutex};
      packets.emplace_back(packet);
    }

    return total_packets;
  };

  // Start a thread for each interface
  for (auto name : interfaces)
    counts.emplace_back(std::async(std::launch::async, func, name));

  // Number of logging iterations, application will then exit
  constexpr auto logging_cycles = 600uz;

  // Start processing the packets, note the other threads are locked out
  for (auto i : std::views::iota(0uz, logging_cycles)) {

    std::this_thread::sleep_for(1s);

    // Map of devices
    static std::map<std::string, ethernet_packet_t> devices;

    auto total_bytes = 0uz;

    // Process the packets
    std::lock_guard lock{packet_mutex};
    for (auto packet : packets) {

      // Store packet size
      total_bytes += packet.length;

      // Store source MAC only
      devices[packet.source.mac] = packet;
    }

    // Clear down the packets
    auto total_packets = packets.size();
    packets.clear();

    // Clear the screen
    std::print("\033[H\033[2J");

    // Print the devices
    for (auto &[mac, device] : devices)
      if (!device.source.ip.empty() || !oui::lookup(mac).empty())
        std::println("{:17} {:15} {:17} {:04x} {}", device.interface,
                     device.source.ip, mac, device.type, oui::lookup(mac));

    // Print summary
    std::println("\n{:02}/{:02} packets: {}\n", i + 1, logging_cycles,
                 total_packets);
  }

  std::println("Stopping {} threads", interfaces.size());

  // Ask all the threads to finish what they're doing
  running = false;

  // Wrap names and return values for convenience
  auto zipped = std::views::zip(interfaces, counts);

  // Wait for all the threads to finish
  for (auto [name, count] : zipped)
    std::println("{:6} {:16}", count.get(), name);
}
