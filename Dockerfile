# Build with: docker build --platfrom linux/amd64 -t shh .
# This is necessary because the build container (deanturpin/dev) is linux/amd64 and the runtime container is multi-platform

# docker buildx create --name multiarch --use
# docker buildx build . -t <your_image_name> -f Dockerfile --platform linux/arm64/v8,linux/amd64 --push

FROM deanturpin/dev as build

WORKDIR /root/shh
COPY . .

RUN make

FROM ubuntu:latest

RUN apt-get update && \
    apt-get install -y figlet iproute2 libpcap0.8t64 && \
    apt-get clean

COPY --from=build /root/shh/build/shh /root/shh/build/
COPY --from=build /root/shh/ieee-oui.txt /root/shh/

WORKDIR /root/shh

CMD figlet deanturpin/shh && \
    ip -brief addr && \
    stdbuf -o0 -e0 build/shh
