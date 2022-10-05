#include <iostream>
#include <memory>
#include <filesystem>

#include <SDL.h>

template<>
class std::default_delete<SDL_Surface>
{
public:
    void operator()(SDL_Surface *ptr) const
    {
        SDL_FreeSurface( ptr );
    }
};

std::unique_ptr<SDL_Surface> loadSurface(const std::filesystem::path& path, const SDL_PixelFormat* pixelFormat);
