all:
	cmake -B build -S src -G Ninja
	cmake --build build --parallel
	build/wispa

entr:
	ls CMakeLists.txt Makefile src/* | entr -cr make

stats:
	sloccount bin/ src/*.cxx src/CMakeLists.txt Makefile | grep -E "SLOC|Cost"

clean:
	$(RM) -r build

capture.pcap:
	tcpdump -c 10 -w $@

format:
	clang-format -i src/*.cxx src/*.h
