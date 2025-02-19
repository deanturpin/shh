FROM deanturpin/dev

WORKDIR /root/shh
COPY . .

RUN make
CMD figlet deanturpin/shh && \
    ip -brief addr && \
    sleep 3s && \
    stdbuf -o0 -e0 build/shh
