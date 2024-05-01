#include <atomic>
#include <cassert>
#include <chrono>
#include <mutex>
#include <pcap.h>
#include <print>
#include <string.h>
#include <string>
#include <string_view>
#include <thread>
#include <vector>
#include <map>
#include <ranges>

// Forward declarations
std::map<std::string, std::string> get_oui();

// Information about devices
struct device_t {
  std::string_view network{};
  std::string ip{"____________"};
  std::string vendor{};
  size_t packets{};
  size_t packet_length{};
  uint16_t packet_type{};
};

// Capture packets from the given network device
auto capture2(std::string_view network_device) {

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

    // Extract MAC address
    auto mac = std::string{};
    for (auto i = size_t{6}; i < 12; ++i)
      mac += std::format("{:02x}", data[i]) + "-";

    // Create a new device to describe this packet
    auto device = device_t{.network = network_device};

    // Copy these data into the outgoing device
    device.network = network_device;
    device.packet_type = data[12] << 8 | data[13];
    device.packet_length = header.len;
    device.packets = 1;

    // Only set IP address if it's a suitable packet type
    if (device.packet_type == 0x0800)
      device.ip = std::format("{}.{}.{}.{}", data[26], data[27], data[28], data[29]);

    // Add device to the list
    devices.emplace(mac, device);
  }

  return devices;
}

int main() {

  using namespace std::chrono_literals;

  std::print("Processing vendor file... ");
  auto oui = get_oui();
  std::println("{} vendors", oui.size());

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

  // Search for MAC addresses
  std::thread sniffer{[&]() {
    while (run) {

      // Capture packets from each network device
      auto dx = capture2(network_devices[0]);
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
  }};

  //   std::vector<std::thread> threads;
  // Report MACs seen and packet count
  auto reporter = std::thread{[&]() {
    while (run) {

      // Clear terminal
      std::print("\033[2J\033[1;1H");

      // Print current time
      auto now = std::chrono::system_clock::now();
      auto now_c = std::chrono::system_clock::to_time_t(now);
      std::println("{}", std::ctime(&now_c));

      // Grab devices and print summary
      {
        std::scoped_lock lock{mac_mutex};
        for (auto [mac, device] : devices) {
          auto vendor = oui.contains(mac.substr(0, 8)) ? oui[mac.substr(0, 8)]
                                                       : mac + " unknown";
          std::println("{}\t{}\t{}", device.ip, device.packets, vendor);
        }
      }

      std::this_thread::sleep_for(1s);
    }

    std::println("Reporter stopped");
  }};

  // Wait for a while
  std::this_thread::sleep_for(60s);

  // Request all threads stop
  run = false;

  // Wait for the threads to finish
  if (sniffer.joinable())
    sniffer.join();

  if (reporter.joinable())
    reporter.join();

  std::println("cya!");
}