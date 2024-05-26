all: ieee-oui.txt
	cmake -B build -S .
	cmake --build build --parallel

run: all
	build/shh

entr:
	ls Makefile src/* | entr -cr make --silent run

stats:
	sloccount bin/ src/*.cxx src/CMakeLists.txt Makefile | grep -E "SLOC|Cost"

clean:
	$(RM) -r build

ieee-oui.txt:
	curl https://standards-oui.ieee.org/oui/oui.txt --output $@

publish:
	mkdir -p public/
	pandoc index.md --standalone --output public/index.html
