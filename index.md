# Careless Wispa: loose lips sink ships

> Why did George Michael get chocolate on his pants? Because he was careless with his Wispa.

## Passive network monitoring

Command line monitoring and summarising of network traffic.

## Building and running in a Docker container

```bash
docker run -it --network host -v .:/run deanturpin/gcc make
```

Some documentation claims you need to use `--cap-add=NET_ADMIN`, but, I mean, it's working for me with just the `--network host` flag.

## Tasks

- [ ] Dump markdown summary to `stderr` on exit
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

<!--

    // struct EthernetHeader {
    //     uint8_t destMac[6];  // Destination MAC address
    //     uint8_t srcMac[6];   // Source MAC address
    //     uint16_t etherType;  // Ethernet type
    // };

IPv4 (0x0800): Indicates that the payload is an IPv4 packet.
IPv6 (0x86DD): Indicates that the payload is an IPv6 packet.
ARP (0x0806): Indicates that the payload is an ARP (Address Resolution Protocol) packet.
VLAN Tagged Frame (0x8100): Indicates the presence of VLAN tagging.
MPLS Unicast (0x8847): Indicates the presence of MPLS (Multiprotocol Label Switching) payload.
MPLS Multicast (0x8848): Indicates the presence of MPLS payload for multicast packets.
LLDP (0x88CC): Indicates the payload is a Link Layer Discovery Protocol frame.


```

  // // Set a filter (optional)
  // struct bpf_program filter;
  // pcap_compile(pcapHandle, &filter, "tcp port 80", 0, PCAP_NETMASK_UNKNOWN);
  // pcap_setfilter(pcapHandsle, &filter);

  // // Start capturing packets
  // std::cout << "Capturing packets..." << std::endl;
  // pcap_loop(pcapHandle, 0, packetHandler, nullptr);

  // // Close the pcap handle when done
  // pcap_close(pcapHandle);

// static_assert(not std::empty(get_quote());


// // file header
// struct pcap_file_header {
//   std::uint32_t magic_number;  // Magic number
//   std::uint16_t version_major; // Major version number
//   std::uint16_t version_minor; // Minor version number
//   std::int32_t thiszone;       // GMT to local correction
//   std::uint32_t sigfigs;       // Accuracy of timestamps
//   std::uint32_t snaplen;       // Max length of captured packets
//   std::uint32_t network;       // Data link type
// };

// // packet header
// struct pcap_packet_header {
//   std::uint32_t ts_sec;   // Timestamp seconds
//   std::uint32_t ts_usec;  // Timestamp microseconds
//   std::uint32_t incl_len; // Number of octets of packet saved in file
//   std::uint32_t orig_len; // Actual length of packet
// };

// std::atexit([]() {
//   std::println("cya!");
//   return 0;
// });
-->

---
