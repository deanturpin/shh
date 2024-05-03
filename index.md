# Careless Wispa

Passive command line monitoring of network traffic.

> Why did George Michael get chocolate on his pants? Because he was careless with his Wispa.

## Build and run

Developed with compilers built from source: g++ 15 and clang++ 19.

```bash
make
```

## Building and running in a Docker container

Using latest everything is not always possible so it can be run in an container. Of course, you need to expose your network to the container, which is considered a security risk by some.

```bash
docker run -it --network host -v .:/run deanturpin/gcc make
```

All looks nice but... it runs happily in the container but doesn't appear to capture the packet type; so the code to capture the IP never triggers. Some documentation claims you need to use `--cap-add=NET_ADMIN` but that' doesn't fix it. To be continued...

---
