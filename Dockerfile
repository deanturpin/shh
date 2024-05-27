FROM deanturpin/dev

WORKDIR /root/shh
COPY . .

RUN make
CMD figlet deanturpin/shh && \
    ip -brief addr && \
    build/shh
