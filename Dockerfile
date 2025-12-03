FROM deanturpin/dev

WORKDIR /root/shh
COPY . .

RUN make
CMD ["sh", "-c", "clear && figlet deanturpin/shh && echo 'Superseded by deanturpin/stooge' && cat /etc/os-release && sleep 2s && stdbuf -o0 -e0 build/shh"]
