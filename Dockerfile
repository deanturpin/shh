FROM deanturpin/dev

WORKDIR /root/shh
COPY . .

RUN make
CMD clear && \
    figlet deanturpin/shh && \
    ip -brief addr && \
    sleep 5s && \
    stdbuf -o0 -e0 build/shh
