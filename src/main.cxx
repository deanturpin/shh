#include "device.h"
#include <algorithm>
#include <atomic>
#include <cassert>
#include <map>
#include <mutex>
#include <print>
#include <string>
#include <thread>
#include <vector>
#include "oui.h"

namespace cap {
std::multimap<std::string, device_t> read(std::string_view);
std::vector<std::string> interfaces();
} // namespace cap

int main() {

  using namespace std::chrono_literals;

  // Get network interfaces
  auto network_interfaces = cap::interfaces();
  assert(not std::empty(network_interfaces));

  std::println("Network interfaces:");
  for (auto d : network_interfaces)
    std::println("\t{}", d);

  std::println("READY");
  std::this_thread::sleep_for(2s);

  // Control the threads
  std::atomic_bool run{true};

  // Shared MAC data
  std::mutex mac_mutex;
  std::map<std::string, device_t> devices;

  // Create container for all threads
  std::vector<std::thread> threads;

  // Search for MAC addresses
  threads.emplace_back([&]() {
    while (run) {

      // Use only the first network interface
      auto dev = network_interfaces.front();

      // Capture packets from the chosen network interface
      auto dx = cap::read(dev);

      // Grab the devices container and update it
      {
        std::scoped_lock lock{mac_mutex};

        // Add devices to the list
        for (auto [mac, device] : dx) {
          devices[mac].packets += device.packets;
          devices[mac].ip = device.ip;
          devices[mac].packet_type = device.packet_type;
        }
      }
    }

    std::println("Sniffer stopped");
  });

  // Report devices seen and packet count
  threads.emplace_back([&]() {
    while (run) {

      // Clear terminal
      std::print("\033[2J\033[1;1H");

      // Print current time
      auto now = std::chrono::system_clock::now();
      auto now_c = std::chrono::system_clock::to_time_t(now);
      std::println("{}", std::ctime(&now_c));

      // Print markdown table header
      std::println("| MAC | IP | Type | Packets | Vendor |");
      std::println("|-|-|-|-|-|");

      // Grab the devices container and print summary
      {
        std::scoped_lock lock{mac_mutex};
        for (auto [mac, device] : devices) {
          auto vendor = oui::lookup(mac);
          std::println("| {} | {:15} | {:04x} | {:6} | {:30} |",
                       mac.substr(0, 8), device.ip, device.packet_type,
                       device.packets, vendor);
        }
      }

      std::this_thread::sleep_for(1s);
    }

    std::println("Reporter stopped");
  });

  // Wait for a while
  std::this_thread::sleep_for(60s);

  // Request all threads stop
  run = false;

  // Wait for the threads to finish
  std::ranges::all_of(threads, [](auto &t) {
    if (t.joinable())
      t.join();

    return true;
  });
}