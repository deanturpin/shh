# Careless Wispa

[![](https://gitlab.com/deanturpin/wispa/badges/main/pipeline.svg)](https://gitlab.com/deanturpin/wispa/-/pipelines)

Passive command line monitoring of network traffic.

See [the repo](https://gitlab.com/deanturpin/wispa).

## Build and run

Just clone the repo and run `make`, which invokes all the usual CMake commands and executes the binary.

## Building and running in a Docker container

Using "latest everything" is not always possible so it can be run in an container. Of course you need to expose your network to the container, which is considered a security risk by some.

```bash
$ docker run -it --network host -v .:/run deanturpin/gcc make
```

All looks nice... and it runs happily in the container... but it doesn't appear to capture the packet type; which makes parsing the packet a little tricky. Some documentation claims you need to use `--cap-add=NET_ADMIN` but that doesn't fix it. _To be continued._

---
