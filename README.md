# shh

Passive command line monitoring of network traffic.

## Basic operation

You need to expose your host network to the container to see much, but at least you don't have to run `sudo` as Docker itself is privileged.

```bash
docker run --network=host deanturpin/shh
```

## Generating traffic

`shh` is passive, but you might like to generate some traffic on your network.

```bash
nmap -A 192.168.1.0/24
```

## Recompiling in-place

The source is deployed with the container, so you can run it interactively and rebuild if you like.

```bash
docker run -it --network=host deanturpin/shh bash
touch src/main.cpp
make run
```

And all the usual `git` commands work, so you can push to my repo (if you're a collaborator) by running `ssh-keygen` and pushing the public key to GitLab.

## Debugging with Visual Studio Code

For an even sweeter development experience you can also connect to the running container with Visual Studio Code.

## clone and build

You can even build without running in a container... imagine! Simply clone the [repo](https://gitlab.com/deanturpin/shh) and run `make run`, which invokes all the usual CMake commands and executes the binary. But you do need all the latest compilers of course.

