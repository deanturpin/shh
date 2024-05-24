FROM deanturpin/dev

# Prepare the build area
WORKDIR /root/shh
COPY . .

# Build the project
RUN make

# Run unbuffered
CMD stdbuf -o0 -e0 build/shh
