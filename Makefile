SDL2_CFLAGS = $(shell sdl2-config --cflags)
CXXFLAGS = $(SDL2_CFLAGS)
LD_FLAGS = $(shell pkg-config --libs SDL2_image SDL2_ttf)

all: sdlplay sdlmouse

OBJ = src/context.o src/texture.o src/surface.o src/font.o

sdlplay: src/main.o $(OBJ)
	$(CXX) -o $@ $^ $(LD_FLAGS)

sdlmouse: src/mouse.o $(OBJ)
	$(CXX) -o $@ $^ $(LD_FLAGS)

clean:
	-rm -f sdlplay
	-rm -f src/*.o

install: all
	mkdir -p ${PREFIX}/bin
	cp -f sdlplay ${PREFIX}/bin
	chmod 755 ${PREFIX}/bin/sdlplay

uninstall:
	rm -f ${PREFIX}/bin/sdlplay

.PHONY: all clean install uninstall
