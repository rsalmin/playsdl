#include <iostream>
#include <memory>
#include <filesystem>

#include <SDL.h>
#include <SDL_mixer.h>

#include "music.hpp"

std::unique_ptr<Mix_Music> loadMusic(const std::filesystem::path& path)
{
    Mix_Music* music = Mix_LoadMUS( path.c_str() );
    if( music == NULL )
    {
        std::cerr <<  "Failed to load music! from " << path << " SDL_mixer Error: " << Mix_GetError();
        return nullptr;
    }

    return std::unique_ptr<Mix_Music>(music);
}

std::unique_ptr<Mix_Chunk> loadChunk(const std::filesystem::path& path)
{
    Mix_Chunk* chunk = Mix_LoadWAV( path.c_str() );
    if( chunk == NULL )
    {
        std::cerr <<  "Failed to load chunk! from " << path << " SDL_mixer Error: " << Mix_GetError();
        return nullptr;
    }

    return std::unique_ptr<Mix_Chunk>(chunk);
}
