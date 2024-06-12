# shh - pcap packet sniffer

See the [Dockerfile](https://github.com/deanturpin/shh/blob/main/Dockerfile) and [issues](https://github.com/deanturpin/shh/issues).

## Basic operation

You have to expose your host network to the container to see what's going on, but at least you don't have to run `sudo` as Docker itself is privileged.

```bash
docker run -it --rm --network=host deanturpin/shh
```

## Generating traffic

`shh` is passive, but you might like to generate some traffic on your network.

```bash
nmap -A 192.168.0.0/24
```

## Recompiling in-place

The source is deployed with the container, so you can run it interactively and rebuild if you like.

```bash
docker run -it --rm --network=host deanturpin/shh bash
touch src/main.cpp
make run
```

And all the usual `git` commands work, so you could push straight back to my repo (if I have let you); or just dump a patch of the diffs to `/tmp` and copy it out of the container.

```bash
git diff > /tmp/diff.patch
```

## Debugging with Visual Studio Code

For an even sweeter development experience you can connect to the running container with Visual Studio Code.

Run executing `bash` (as above) and then connect your IDE to the running container. Open `/root/shh` and install extensions "CMake Tools" and "C++" to build and debug.

## Debugging with Codespaces

Run the following in the web IDE terminal, it will build and run when a file changes.

```bash
docker run --rm -it --network=host -v .:/root/shh deanturpin/dev make --directory /root/shh entr
```

## Build locally

You could even build without running in a container... imagine! Simply clone the [repo](https://github.com/deanturpin/shh) and run `make run` to invokes all the usual CMake commands and also executes the binary. But you will need all the latest compilers installed, of course.

## Design

The main thread starts a pcap logging thread for each interface; each thread then reads packets from a single interface and pushes captured packets to a shared container. Peridoically the main thread processes and empties the shared resource and prints a summary of devices encountered: basically the MAC address and associated IP/flags. The main thread runs for a fixed duration and then signals all the threads to stop before joining them to ensure no resources are leaked.

Something that drove me mad for a while was not setting the pcap reads to non-blocking; whilst it worked fine on my macOS laptop, on Linux the threads would occasionally block and leave the main thread hanging.

```cpp
pcap_setnonblock(pcap, 1, errbuf);
```

## C++ features of note

This project is my current testbed for the latest C++ features. I do run a [nightly latest GCC build](https://hub.docker.com/r/deanturpin/gcc) from source but GCC 14 is readily available on Linux and macOS so that will suffice.

### GCC flags

Firstly GCC 14 now accepts the `-std=c++26` flag; it has also introduced `-Wnrvo` to warn when the compiler is unable to perform return value optimisation.

### "print" header

I've been enjoying `std::format` for a while but now we have `std::print`! And also the variant with the implicit newline.

```cpp
std::println("https://github.com/deanturpin/shh/commit/{}\n", GIT_HASH);
```

### size_t literals

```cpp
constexpr auto logging_cycles = 60uz;
```

### Zipped views

```cpp
auto zipped = std::views::zip(interfaces, counts);

for (const auto& [interface, count] : zipped) {
    std::println("{}: {}", interface, count);
}
```

### Static assertions as compile-time unit testing

```cpp
static_assert(is_print('~'));
static_assert(not is_print('\n'));
static_assert(not std::has_virtual_destructor_v<packet_t>);
```

### Designated initialisers

Name the members you want to initialise.

```cpp
return {
    .interface = interface,
    .source = {.mac = source_mac, .ip = source_ip},
    .destination = {.mac = destination_mac, .ip = destination_ip},
    .type = std::byteswap(eth.packet_type),
    .length = header.len,
};
```

### Ranges

```cpp
if (not std::ranges::any_of(
        excludes, [name](auto excluded) { return name == excluded; }))
    names.emplace_back(name);
```

### Threads

I've used `std::async` to create each logging thread; which also provides a natural synchronisation point when you wait for the return value from each thread.

```cpp
  for (auto name : interfaces)
    counts.emplace_back(std::async(std::launch::async, func, name));
```
