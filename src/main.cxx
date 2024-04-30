#include "pcap.h"
#include <filesystem>
#include <fstream>
#include <print>
#include <string>
// #include <ranges>

int main() {
  std::println("Careless Wispa");

  // Open all the pcap files in the current directory

  // Process each file in the directory, skipping non-csv files
  const auto files = std::filesystem::directory_iterator{"."};

  for (auto file : files) {
    if (file.path().extension() != ".pcap")
      continue;

    std::println("Processing {}", file.path().string());


    // Open example.pcap file
    std::ifstream in(file.path().string(), std::ios::binary);

    // Read pcap file header
    pcap_file_header file_header;
    in.read(reinterpret_cast<char*>(&file_header), sizeof(pcap_file_header));

    std::println("Magic number: {:x}", file_header.magic_number);
    std::println("Version major: {}", file_header.version_major);
    std::println("Version minor: {}", file_header.version_minor);
    std::println("Thiszone: {}", file_header.thiszone);
    std::println("Sigfigs: {}", file_header.sigfigs);
    std::println("Snaplen: {}", file_header.snaplen);
    std::println("Network: {}", file_header.network);
    
    // // Check if file header is valid
    // if (file_header.magic_number != 0xa1b2c3d4) {
    //   std::println("Error: Invalid file header");
    //   return 1;
    // }

    // // Read pcap packet header
    // pcap_packet_header packet_header;
    // in.read(reinterpret_cast<char*>(&packet_header), sizeof(pcap_packet_header));

    // // Check if packet header is valid
    // if (in.eof()) {
    //   std::println("no more packets to read");
    //   return 1;
    // }    

  }
}