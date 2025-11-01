MAKEFLAGS += --silent

all:
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

publish:
	mkdir -p public/
	pandoc index.md --standalone --output public/index.html

package:
	cpack
	dpkg --contents *.deb

lint:
	docker run --rm -i hadolint/hadolint < Dockerfile || true
	markdownlint *.md || true
