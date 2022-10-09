SDL2_CFLAGS = $(shell sdl2-config --cflags)
CXXFLAGS = $(SDL2_CFLAGS)
LD_FLAGS = $(shell pkg-config --libs SDL2_image SDL2_ttf SDL2_mixer)

all: sdldull sdlmouse

OBJ = src/context.o src/texture.o src/surface.o src/font.o src/music.o src/fps_counter.o

sdldull: src/dull.o $(OBJ)
	$(CXX) -o $@ $^ $(LD_FLAGS)

sdlmouse: src/mouse.o $(OBJ)
	$(CXX) -o $@ $^ $(LD_FLAGS)

clean:
	-rm -f sdldull
	-rm -f sdlmouse
	-rm -f src/*.o

install: all
	mkdir -p ${PREFIX}/bin
	cp -f sdldull ${PREFIX}/bin
	chmod 755 ${PREFIX}/bin/sdldull
	cp -f sdlmouse ${PREFIX}/bin
	chmod 755 ${PREFIX}/bin/sdlmouse

uninstall:
	rm -f ${PREFIX}/bin/sdldull
	rm -f ${PREFIX}/bin/sdlmouse

.PHONY: all clean install uninstall
