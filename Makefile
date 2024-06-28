MAKEFLAGS += --silent

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
	if [ -f /usr/share/arp-scan/ieee-oui.txt ]; then \
		cp /usr/share/arp-scan/ieee-oui.txt $@; \
	else \
		curl https://standards-oui.ieee.org/oui/oui.txt --output $@; \
	fi

publish:
	mkdir -p public/
	pandoc index.md --standalone --output public/index.html
