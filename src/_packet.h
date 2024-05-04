#pragma once

#include "capture.h"
#include "device.h"
#include "oui.h"
#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <mutex>
#include <pcap.h>
#include <print>
#include <ranges>
#include <thread>

class capture {

  std::atomic_bool run{true};
  std::mutex buffer_mutex;
  std::thread thread{};
  std::string interface_{};
  // std::vector<device_t> buffer{};
  size_t packets{};
  pcap_t *pcap = nullptr;

  // Open the network interface in promiscuous mode
  void init(std::string_view interface) {
    interface_ = interface;
    char errbuf[256];
    pcap =
        pcap_open_live(std::string{interface_}.c_str(), 65535, 1, 1000, errbuf);
  }

  void shutdown() {
    if (pcap != nullptr)
      pcap_close(pcap);
  }

public:
  // Start capture on a single interface
  void start(std::string_view interface) {

    // Initialise the capture device
    init(interface);

    // Listen until the thread is told to stop
    thread = std::thread([this] {
      while (run) {

        // Read packets
        pcap_pkthdr header;
        const u_char *data = pcap_next(pcap, &header);

        // If we didn't receive any data, hold off and try again
        if (data == nullptr) {
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
          continue;
        }

        {
          std::scoped_lock lock{buffer_mutex};
          packets += header.len;
        }

        // auto dx = cap::read(interface);

        // {
        //   // std::scoped_lock lock{buffer_mutex};

        //   // for (auto [_, device] : dx)
        //   //   buffer.push_back(device);
        // }

        // yeild to other threads
        // std::this_thread::yield();

        // sleep for a while
        // std::this_thread::sleep_for(std::chrono::seconds(1));
      }

      std::println("Thread stopped for {}", interface_);
    });
  }

  // Shut down capture
  ~capture() {
    std::println("Capture stopped on {}", interface_);
    run = false;
    if (thread.joinable())
      thread.join();

    shutdown();
  }

  // Get the buffer size
  size_t size() {
    std::scoped_lock lock{buffer_mutex};
    // return buffer.size();
    return packets;
  }

  // Get the interface name
  std::string_view interface() const { return interface_; }

  // Delete other constructors
  // capture(const capture &) noexcept = delete;
  // capture(capture &&) noexcept = delete;
  capture &operator=(const capture &) noexcept = delete;
  capture &operator=(capture &&) noexcept = delete;
};