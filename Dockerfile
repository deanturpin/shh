FROM deanturpin/dev

WORKDIR /root/shh
COPY . .

# Build the project
RUN make

# Run unbuffered
CMD stdbuf -o0 -e0 build/shh
