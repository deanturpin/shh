FROM deanturpin/dev

WORKDIR /root/shh
COPY . .

RUN make
CMD figlet deanturpin/shh && \
    ip -brief addr && \
    stdbuf -o0 -e0 build/shh
