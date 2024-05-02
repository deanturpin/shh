#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <map>
#include <mutex>
#include <pcap.h>
#include <print>
#include <ranges>
#include <string.h>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

// Forward declarations
namespace oui {
std::string lookup(const std::string_view);
}

// Information about devices
struct device_t {
  size_t packets{};
  size_t packet_length{};
  uint16_t packet_type{};
  std::string_view network{};
  std::string ip{};
  std::string vendor{};
};

// Capture packets from the given network device
auto capture(std::string_view network_device) {

  auto devices = std::multimap<std::string, device_t>{};

  // Open the network device in promiscuous mode
  char errbuf[256];
  pcap_t *pcap = pcap_open_live(std::string{network_device}.c_str(), 65535, 1,
                                1000, errbuf);

  if (pcap == nullptr) {
    std::println("Error opening network device: '{}'",
                 std::string_view(errbuf));
    return devices;
  }

  // Process number of packets
  for (auto _ : std::views::iota(0, 20)) {

    // Read packets
    pcap_pkthdr header;
    const u_char *data = pcap_next(pcap, &header);

    if (data == nullptr)
      continue;

    // Create a new device to describe this packet
    auto device_source = device_t{.network = network_device};
    auto device_dest = device_t{.network = network_device};

    // Extract MAC addresses
    auto mac_source = std::string{};
    for (auto i = size_t{6}; i < 12; ++i)
      mac_source += std::format("{:02x}", data[i]) + "-";

    auto mac_dest = std::string{};
    for (auto i = size_t{0}; i < 6; ++i)
      mac_dest += std::format("{:02x}", data[i]) + "-";

    // Copy these data into the outgoing device
    device_source.network = network_device;
    device_source.packet_type = data[12] << 8 | data[13];
    device_source.packet_length = header.len;
    device_source.packets = 1;

    // Copy these data into the outgoing device
    device_dest.network = network_device;
    device_dest.packet_type = data[12] << 8 | data[13];
    device_dest.packet_length = header.len;
    device_dest.packets = 1;

    // Only set IP address if it's a suitable packet type
    if (device_source.packet_type == 0x0800)
      device_source.ip =
          std::format("{}.{}.{}.{}", data[26], data[27], data[28], data[29]);

    if (device_dest.packet_type == 0x0800)
      device_dest.ip =
          std::format("{}.{}.{}.{}", data[30], data[31], data[32], data[33]);

    // Add device to the list
    devices.emplace(mac_source, device_source);
    devices.emplace(mac_dest, device_dest);
  }

  return devices;
}

int main() {

  using namespace std::chrono_literals;

  // List network devices
  std::println("Network devices:");

  pcap_if_t *alldevs;
  char errbuf[256];

  if (pcap_findalldevs(&alldevs, errbuf) == -1) {
    std::println("Error in pcap_findalldevs: {}", errbuf);
    return 1;
  }

  std::vector<std::string> network_devices{};

  for (pcap_if_t *d = alldevs; d != nullptr; d = d->next)
    network_devices.push_back(d->name);

  assert(not std::empty(network_devices));

  for (auto d : network_devices)
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

      // If there's an "any" device then use it
      // Otherwise the first one will do
      auto it = std::ranges::find(network_devices, "any");
      auto dev = it != network_devices.end() ? *it : *std::cbegin(network_devices);

      // Capture packets from the chosen network device
      auto dx = capture(dev);

      {
        std::scoped_lock lock{mac_mutex};

        // Add devices to the list
        for (auto [mac, device] : dx) {
          devices[mac].packets += device.packets;
          devices[mac].ip = device.ip;
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
      std::println("| MAC | IP | Packets | Vendor |");
      std::println("|-|-|-|-|");

      // Grab devices and print summary
      {
        std::scoped_lock lock{mac_mutex};
        for (auto [mac, device] : devices) {
          auto vendor = oui::lookup(mac);
          std::println("| {} | {:15} | {:6} | {:30} |", mac.substr(0, 8),
                       device.ip, device.packets, vendor);
        }
      }

      std::this_thread::sleep_for(1s);
    }

    std::println("Reporter stopped");
  });

  // Wait for a while
  std::this_thread::sleep_for(30 * 60s);

  // Request all threads stop
  run = false;

  // Wait for the threads to finish
  std::ranges::all_of(threads, [](auto &t) {
    if (t.joinable())
      t.join();

    return true;
  });
}