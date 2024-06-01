# shh

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

Run executing `bash` and then connect your IDE to the container. Open `/root/shh` and install CMake Tools and C++ to build and debug.

```bash
docker run -it --rm --network=host deanturpin/shh bash
```
## clone and build

You could even build without running in a container... imagine! Simply clone the [repo](https://github.com/deanturpin/shh) and run `make run`, which invokes all the usual CMake commands and executes the binary. But you will need all the latest compilers installed of course.

## Design

The main thread starts a pcap logging thread for each interface; each thread then reads packets from a single interface and pushes captured packets to a shared container. Peridoically the main thread processes and empties the shared resource and prints a summary of devices encountered: basically the MAC address and associated IP/flags. The main thread runs for a fixed duration and then signals all the threads to stop before joining them to ensure no resources are leaked.

Something that drove me mad for a while was not setting the pcap reads to non-blocking; whilst it worked fine on my macOS laptop, on Linux the threads would occasionally block and leave the main thread hanging. However, it did lead me to reimplement it using plain old `std::thread`, `std::jthread` with built-it stop token, `std::for_each` with `std::execution::par` and finally `std::async` with `std::launch::async`; which I think is quite nice to read: waiting for the return value from the thread is a natural way to synchronise with all the stopping threads.

```cpp
pcap_setnonblock(pcap, 1, errbuf);
```