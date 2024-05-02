all: oui.txt
	cmake -B build -S src -G Ninja
	cmake --build build --parallel
	build/wispa

entr:
	ls Makefile src/* | entr -cr make

stats:
	sloccount bin/ src/*.cxx src/CMakeLists.txt Makefile | grep -E "SLOC|Cost"

clean:
	$(RM) -r build

oui.txt:
	curl -O https://standards-oui.ieee.org/oui/oui.txt

format:
	clang-format -i src/*.cxx src/*.h
