SDL2_CFLAGS = $(shell sdl2-config --cflags)
CXXFLAGS = $(SDL2_CFLAGS)
LD_FLAGS = $(shell pkg-config --libs SDL2_image SDL2_ttf)

all: sdlplay

OBJ = src/main.o src/context.o

sdlplay: $(OBJ)
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
