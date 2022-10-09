#include <iostream>
#include <memory>
#include <filesystem>

#include <SDL.h>
#include <SDL_ttf.h>

template<>
class std::default_delete<TTF_Font>
{
public:
    void operator()(TTF_Font *ptr) const
    {
        TTF_CloseFont( ptr );
    }
};


std::unique_ptr<TTF_Font> loadFont(const std::filesystem::path& path);

using Font = std::unique_ptr<TTF_Font>;
