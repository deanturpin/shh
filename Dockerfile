FROM deanturpin/dev

WORKDIR /root/shh
COPY . .

RUN make
CMD ["sh", "-c", "clear && figlet deanturpin/shh && cat /etc/os-release && sleep 2s && stdbuf -o0 -e0 build/shh"]
