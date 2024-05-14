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

---
