#include <iostream>
#include <memory>
#include <filesystem>

#include <SDL.h>
#include <SDL_mixer.h>

template<>
class std::default_delete<Mix_Music>
{
public:
    void operator()(Mix_Music *ptr) const
    {
        Mix_FreeMusic( ptr );
    }
};

template<>
class std::default_delete<Mix_Chunk>
{
public:
    void operator()(Mix_Chunk *ptr) const
    {
        Mix_FreeChunk( ptr );
    }
};

using MixMusic = std::unique_ptr<Mix_Music>;
using MixChunk = std::unique_ptr<Mix_Chunk>;

std::unique_ptr<Mix_Music> loadMusic(const std::filesystem::path& path);
std::unique_ptr<Mix_Chunk> loadChunk(const std::filesystem::path& path);
