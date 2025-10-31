#include "dns.h"
#include <arpa/inet.h>
#include <future>
#include <map>
#include <mutex>
#include <netdb.h>
#include <sys/socket.h>

namespace dns {

// Cache of IP to hostname mappings
// Key: IP address string
// Value: future containing hostname (or empty string if lookup failed/pending)
namespace {
auto cache_mutex = std::mutex{};
auto cache = std::map<std::string, std::shared_future<std::string>>{};
} // namespace

// Perform the actual blocking reverse DNS lookup
std::string perform_lookup(std::string ip) {
  sockaddr_in sa{};
  sa.sin_family = AF_INET;

  // Convert IP string to binary
  if (inet_pton(AF_INET, ip.c_str(), &sa.sin_addr) != 1)
    return {};

  char hostname[NI_MAXHOST]{};

  int result =
      getnameinfo(reinterpret_cast<sockaddr *>(&sa), sizeof(sa), hostname,
                  NI_MAXHOST, nullptr, 0, NI_NAMEREQD);

  return (result == 0) ? hostname : std::string{};
}

// Non-blocking reverse lookup with caching
std::string reverse_lookup(std::string_view ip) {
  if (ip.empty())
    return {};

  auto ip_str = std::string{ip};

  // Check cache
  auto lock = std::lock_guard{cache_mutex};

  // If we've seen this IP before
  if (cache.contains(ip_str)) {
    auto &future = cache.at(ip_str);

    // Check if the lookup is complete
    if (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
      return future.get();

    // Still pending, return empty string
    return {};
  }

  // Start async lookup and cache the future
  cache[ip_str] = std::async(std::launch::async, perform_lookup, ip_str);

  // Return empty string immediately (non-blocking)
  return {};
}

} // namespace dns
