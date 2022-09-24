SDL2_CFLAGS = $(shell sdl2-config --cflags)
CXX_FLAGS = $(SDL2_CFLAGS)
LD_FLAGS = -lSDL2

all: sdlplay

sdlplay: main.cpp
	$(CXX) -o $@ $(CXX_FLAGS) $^ $(LD_FLAGS)

clean:
	-rm -f sdlplay

install: all
	mkdir -p ${PREFIX}/bin
	cp -f sdlplay ${PREFIX}/bin
	chmod 755 ${PREFIX}/bin/sdlplay

uninstall:
	rm -f ${PREFIX}/bin/sdlplay

.PHONY: all clean install uninstall
