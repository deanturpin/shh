#include "dns.h"
#include "oui.h"
#include "packet.h"
#include "types.h"
#include <atomic>
#include <cassert>
#include <chrono>
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
  auto running = std::atomic_bool{true};

  // Container for the promises
  // These return counts of packets captured on each interface
  auto counts = std::vector<std::future<size_t>>{};

  using namespace std::chrono_literals;

  // The capture routine
  auto func = [&](auto name) {
    // Return the number of packets captured
    auto total_packets = 0uz;

    // Create capture object
    auto capture = cap::packet_t{name};

    // Capture packets until told to stop
    while (running) {

      // Read one packet
      ethernet_packet_t packet = capture.read();

      // If it's invalid, yield and skip it
      if (packet.source.mac.empty()) {
        std::this_thread::sleep_for(1ms);
        continue;
      }

      ++total_packets;

      // Otherwise store the packet
      auto lock = std::lock_guard{packet_mutex};
      packets.emplace_back(packet);
    }

    return total_packets;
  };

  // Start a thread for each interface
  for (auto name : interfaces)
    counts.emplace_back(async(std::launch::async, func, name));

  // Number of logging iterations, application will then exit
  constexpr auto cycles = std::views::iota(0uz, 600uz);

  // Start processing the packets, note the other threads are locked out
  for (auto i : cycles) {

    std::this_thread::sleep_for(1s);

    // Consolidated packet structure
    struct device_t {
      std::string interface{};
      std::string ip{};
      std::string hostname{};
      std::string mac{};
      std::string vendor{};
      uint16_t type{};
    };

    // Map of devices
    static auto devices = std::vector<device_t>{};

    // Process the packets
    auto lock = std::lock_guard{packet_mutex};
    for (auto packet : packets) {

      if (packet.source.mac.empty())
        continue;

      // Find the device
      auto it = std::ranges::find_if(devices, [&](auto &device) {
        return device.mac == packet.source.mac;
      });

      // If it doesn't exist, create it
      if (it == devices.end()) {
        devices.emplace_back(device_t{packet.interface, packet.source.ip,
                                       dns::reverse_lookup(packet.source.ip),
                                       packet.source.mac,
                                       oui::lookup(packet.source.mac),
                                       packet.type});
        continue;
      }

      // Update the device, but only the IP if it's not already set
      if (not packet.source.ip.empty())
        it->ip = packet.source.ip;

      // Update hostname if we don't have it yet
      if (it->hostname.empty())
        it->hostname = dns::reverse_lookup(it->ip);

      // Update type
      it->type = packet.type;
    }

    // Clear down the packets
    auto total_packets = packets.size();
    packets.clear();

    // Clear the screen
    std::print("\033[H\033[2J");

    // Report current time
    std::println("{}", std::chrono::system_clock::now());
    std::println("superseded by deanturpin/stooge\n");

    // Print the devices
    for (auto device : devices)
      std::println("{:17} {:15} {:20} {:17} {:04x} {}", device.interface,
                   device.ip, device.hostname, device.mac, device.type,
                   device.vendor);

    // Print summary
    std::println("\n{:3}/{:03} packets: {}\n", i + 1,
                 std::ranges::distance(cycles), total_packets);
  }

  // Ask all the threads to finish what they're doing
  std::println("Stopping {} threads", interfaces.size());
  running = false;

  // Wrap names and return values for convenience
  auto zipped = std::views::zip(interfaces, counts);

  // Wait for all the threads to finish
  for (auto [name, count] : zipped)
    std::println("\t{:16} {:6}", name, count.get(), name);
}
