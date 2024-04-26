all:
	cmake -B build -S src -G Ninja
	cmake --build build --parallel
	time build/wispa

entr:
	ls CMakeLists.txt Makefile src/* | entr -cr make clean all

stats:
	sloccount bin/ src/*.cxx src/CMakeLists.txt Makefile | grep -E "SLOC|Cost"

clean:
	$(RM) -r build