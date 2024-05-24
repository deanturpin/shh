# Things to do

## Tasks

This is the bit that doesn't move properly.

```cpp
  auto captures = std::vector<packet>{};
  for (auto &interface : network_interfaces)
    captures.emplace_back(interface);
```

```cpp
  ethernet_header &eth2 =
      *reinterpret_cast<ethernet_header *>(const_cast<u_char *>(data));
```

## Running in a container on macOS

```bash
warning: linux_ptrace_test_ret_to_nx: Cannot PTRACE_GETREGS: Input/output error
warning: linux_ptrace_test_ret_to_nx: PC 0x900000000 is neither near return address 0x7ffff8d78000 nor is the return instruction 0x555555965971!
Couldn't get CS register: Input/output error.
```

## Todo

- [ ] default make op should also run the binary, need target just to build
- [ ] Use README.md for Docker instructions
- [ ] Can repo define which VSCode extensions are required?
- [ ] make dev container offer latest gcc by default
- [ ] build in root dir
- [x] rebrand to "shh"?
- [ ] Shut down nicely on ctrl-c
- [ ] fetch the oui data into build dir
- [ ] Trial md table for results
- [ ] check if any new interfaces have been added
- [ ] can we snoop bluetooth?
- [ ] consolidate strip and sanitise
- [x] oui::lookup should probably return empty string if not found
- [ ] can packet_t constructors be opened up?
- [ ] dns lookup for IP addresses
- [ ] Does libpcap have all the headers structures?
- [x] Download something large rather than ping (or maybe apt update?)
- [ ] use reference rather than pointer for memory mapping
- [ ] retry building gcc
- [x] try gcc13 with fmtlib
- [ ] How to static link? Does it mean it runs on Focal?
- [ ] unique pointers for pcap bits
- [x] install xcode - https://developer.apple.com/xcode/cpp/#c++23
- [ ] Make utilities `constexpr`
- [ ] assert source mac is non-zero
- [ ] build and debug in Docker/VSCode
- [x] Refactor into one thread for reading packets and another for consolidating
- [x] Review non-blocking logger thread
- [ ] Add rate method (store last access time in class)
- [ ] Read from a thread should return from the write head back to the previous read head (or 200 packets, whichever is larger) 
- [ ] Show summary of packet types alongside each interface
- [x] Report bits/second on each interface
- [ ] use apt oui if on Debian
- [ ] Dump markdown summary on exit
- [ ] Version reporting (git tag, cmake version)
- [x] Why does Docker/Ubuntu container not display packet types? Is it the "any" interface?
- [ ] Animate unknown IP addresses
- [ ] Search for the OUI file in common locations, otherwise download it
- [ ] Start typing to filter
- [ ] List number of packets on each interface (maybe in different threads?)
- [ ] Don't scroll display, jump to the top and overwrite
- [ ] Lock reporter for reading only (shared_mutex)
- [ ] Trial coroutines
- [ ] Link to `mermaid.live` diagram of captured packets
- [ ] Maybe have a static section for reporting and a dynamic section for live data: is this going down the ncurses route?
- [ ] How to deploy? Package or container?
- [ ] Why use `stop.store(true);` over plain `stop`?
- [x] hide the threads and use parallel for_each
- [x] move capture interfaces into packet class
- [x] Ethernet header is common data structure
- [x] Fill array of ethernet header and stop
- [x] create types header
- [x] use syncstream instead
- [x] Should the mac be in the device info?
- [x] Totals for each vendor
- [x] pcap include should be limited to one file
- [x] Fix columns in output (with `std::print`?)
- [x] List network interfaces
- [x] Lookup MAC addresses in a vendor database
- [x] Use threads
- [x] Print random quiet quote
- [x] Read from a file
- [x] Read live data in promiscuous mode
- [x] Develop on macOS
- [x] Use latest C++
- [x] Use CMake
- [x] Use latest clang

## Mystery OUI

```
00-40-00   (hex)		PCI COMPONENTES DA AMZONIA LTD
004000     (base 16)		PCI COMPONENTES DA AMZONIA LTD
				RUA JOSEF KRYSS
				  01140  BRASIL
				BR

QSC LLC 00:60:74
```

## Snippets

```cpp
IPv4 (0x0800): Indicates that the payload is an IPv4 packet.
IPv6 (0x86DD): Indicates that the payload is an IPv6 packet.
ARP (0x0806): Indicates that the payload is an ARP (Address Resolution Protocol) packet.
VLAN Tagged Frame (0x8100): Indicates the presence of VLAN tagging.
MPLS Unicast (0x8847): Indicates the presence of MPLS (Multiprotocol Label Switching) payload.
MPLS Multicast (0x8848): Indicates the presence of MPLS payload for multicast packets.
LLDP (0x88CC): Indicates the payload is a Link Layer Discovery Protocol frame.

  // Set a filter (optional)
  struct bpf_program filter;
  pcap_compile(pcapHandle, &filter, "tcp port 80", 0, PCAP_NETMASK_UNKNOWN);
  pcap_setfilter(pcapHandsle, &filter);

  // Start capturing packets
  std::cout << "Capturing packets..." << std::endl;
  pcap_loop(pcapHandle, 0, packetHandler, nullptr);

  // Close the pcap handle when done
  pcap_close(pcapHandle);

static_assert(not std::empty(get_quote());

std::atexit([]() {
  std::println("cya!");
  return 0;
});
```
