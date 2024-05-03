# Things to do

## Tasks

- [ ] Make utilities `constexpr`
- [ ] Dump markdown summary on exit
- [ ] Version control
- [ ] Refactor into one thread for reading packets and another for consolidating
- [ ] Why does Docker/Ubuntu container not display packet types? Is it the "any" interface?
- [ ] Add schema for table
- [ ] Animate unknown IP addresses
- [ ] Search for the OUI file in common locations, otherwise download it
- [ ] Start typing to filter
- [ ] List number of packets on each interface (maybe in different threads?)
- [ ] Show summary of packet types
- [ ] Don't scroll display, jump to the top and overwrite
- [ ] Lock reporter for reading only (shared_mutex)
- [ ] Trial coroutines
- [ ] Link to `mermaid.live` diagram of captured packets
- [ ] Maybe have a static section for reporting and a dynamic section for live data: is this going down the ncurses route?
- [ ] How to deploy? Package or container?
- [ ] Why use `stop.store(true);` over plain `stop`?
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