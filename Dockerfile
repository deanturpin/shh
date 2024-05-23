FROM deanturpin/gcc

# Prepare the build area
WORKDIR /run
COPY . .

# Build the project
RUN make

# Run unbuffered
CMD stdbuf -o0 -e0 build/wispa
