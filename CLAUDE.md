# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

`shh` is a pcap-based network packet sniffer written in modern C++26. It captures packets from all network interfaces, extracts device information (MAC addresses, IPs, vendor lookups), and displays a real-time summary of devices on the network.

## Build and Run Commands

```bash
# Build the project (uses CMake via Makefile)
make

# Build and run
make run

# Watch mode: rebuild and run on file changes
make entr

# Create Debian package
make package

# Clean build artefacts
make clean
```

## Docker Development

Primary development workflow uses Docker with the `deanturpin/dev` base image:

```bash
# Run the sniffer
docker run -it --rm --network=host deanturpin/shh

# Interactive development with rebuilding
docker run -it --rm --network=host deanturpin/shh bash
make run

# Codespaces watch mode
docker run --rm -it --network=host -v .:/root/shh deanturpin/dev make --directory /root/shh entr
```

## Architecture

### Threading Model

- **Main thread**: Processes captured packets, updates device list, and displays output
- **Capture threads**: One `std::async` thread per network interface, each reading packets from a single interface using pcap
- **Synchronisation**: Shared packet vector protected by mutex; threads push packets, main thread periodically processes and clears

### Critical pcap Detail

Always set pcap to non-blocking mode with `pcap_setnonblock(pcap, 1, errbuf)`. Without this, threads can block on Linux, causing the main thread to hang. See [packet.cxx:28-29](src/packet.cxx#L28-L29).

### Key Components

- **cap::packet_t** ([packet.h](src/packet.h), [packet.cxx](src/packet.cxx)): RAII wrapper for pcap capture on a single interface. Non-copyable, non-movable. The `read()` method returns `ethernet_packet_t` structs.
- **cap::interfaces()** ([packet.cxx:109-134](src/packet.cxx#L109-L134)): Returns list of network interfaces, excluding loopback and virtual devices
- **oui::lookup()** ([oui.h](src/oui.h)): MAC address vendor lookup
- **ethernet_packet_t** ([types.h](src/types.h)): Data structure containing interface name, source/destination MAC and IP, packet type, and length

### Main Loop

[main.cxx:80-145](src/main.cxx#L80-L145) runs for 600 cycles (10 minutes), processing packets every second: acquires mutex lock, updates device list, clears packet buffer, and prints summary with device count and packet statistics.

## C++ Requirements

- **Compiler**: GCC 14 (specified in [CMakeLists.txt:11-12](CMakeLists.txt#L11-L12))
- **Standard**: C++26 with flags `-std=c++26 -Wnrvo`
- **Dependencies**: libpcap

### Modern C++ Features Used

- `std::print` and `std::println` from `<print>` header
- `size_t` literals (e.g., `60uz`)
- Designated initialisers
- `std::ranges` and `std::views` (zipped views, iota)
- `std::async` for threading
- `static_assert` for compile-time unit testing
- RAII patterns with deleted copy/move constructors

## Build System

- **CMake** minimum version 3.29
- Git hash compiled into binary via `GIT_HASH` preprocessor macro
- CPack configured for Debian package generation
- Makefile wrapper provides convenient commands

## CI/CD

GitHub Actions workflow ([.github/workflows/ci.yml](.github/workflows/ci.yml)) builds and packages on push/PR to main using custom runner `deanturpin/dev`.

## Code Style

- Use structured bindings and ranges where appropriate
- Leverage `static_assert` for compile-time constraints
- Keep RAII discipline for resource management
- Use designated initialisers for struct construction
