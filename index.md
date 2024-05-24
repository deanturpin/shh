# Careless Wispa

[![](https://gitlab.com/deanturpin/wispa/badges/main/pipeline.svg)](https://gitlab.com/deanturpin/wispa/-/pipelines)

Passive command line monitoring of network traffic.

See [the repo](https://gitlab.com/deanturpin/wispa).

## Build and run

Just clone the repo and run `make`, which invokes all the usual CMake commands and executes the binary.

## Running the Docker image

See [Docker](https://hub.docker.com/r/deanturpin/wispa). Note you need to expose your host network to the container.

```bash
docker run --network=host deanturpin/wispa
```

## Generating traffic

`shh` is passive, but you might like to generate some traffic on your network.

```bash
nmap -A 192.168.1.0/24
```

## Recompiling in-place

The source is deployed with the container, so you can run it interactively and rebuild.

```bash
docker run -it --network=host deanturpin/shh bash
```

And all the usual `git` commands work, so you can push to my repo (if you're a collaborator) by running `ssh-keygen` and pushing the public key to GitLab.

## Debugging with Visual Studio Code

For an even sweeter development experience you can also connect to the running container with Visual Studio Code.
