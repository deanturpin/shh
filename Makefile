all: oui.txt
	cmake -B build -S src -G Ninja
	cmake --build build --parallel

run: all
	build/wispa

entr:
	ls Makefile src/* | entr -cr make --silent run

stats:
	sloccount bin/ src/*.cxx src/CMakeLists.txt Makefile | grep -E "SLOC|Cost"

clean:
	$(RM) -r build

oui.txt:
	curl -O https://standards-oui.ieee.org/oui/oui.txt
