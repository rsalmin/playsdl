
SDL2_CFLAGS = $(shell sdl2-config --cflags)

CXX_FLAGS = $(SDL2_CFLAGS)

LD_FLAGS = -lSDL2

main: main.cpp
	$(CXX) -o $@ $(CXX_FLAGS) $^ $(LD_FLAGS)

clean:
	-rm -f main
