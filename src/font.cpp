#include <iostream>
#include <memory>
#include <filesystem>

#include <SDL.h>
#include <SDL_ttf.h>

#include "font.hpp"

std::unique_ptr<TTF_Font> loadFont(const std::filesystem::path& path)
{
    TTF_Font *font = TTF_OpenFont( path.c_str(), 28);
    if ( NULL == font )
    {
        std::cerr << "Failed to load lazy font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return nullptr;
    }

    return std::unique_ptr<TTF_Font>(font);
}
