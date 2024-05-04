# Careless Wispa

[![](https://gitlab.com/deanturpin/wispa/badges/main/pipeline.svg)](https://gitlab.com/deanturpin/wispa/-/pipelines)

Passive command line monitoring of network traffic.

> Why did George Michael get chocolate on his pants? Because he was careless with his Wispa.

See [the repo](https://gitlab.com/deanturpin/wispa).

## Build and run

Developed with compilers built from source: g++ 15 and clang++ 19.

Just clone the repo and run `make`, which invokes all the usual CMake commands and runs the exe.

## Building and running in a Docker container

Using "latest everything" is not always possible so it can be run in an container. Of course you need to expose your network to the container, which is considered a security risk by some.

```bash
docker run -it --network host -v .:/run deanturpin/gcc make
```

All looks nice... and it runs happily in the container... but it doesn't appear to capture the packet type; which makes parsing the packet a little tricky. Some documentation claims you need to use `--cap-add=NET_ADMIN` but that' doesn't fix it. To be continued.

---
