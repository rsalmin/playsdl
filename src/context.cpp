#include <memory>
#include <optional>
#include <cassert>
#include <iostream>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "context.hpp"

std::unique_ptr<SDL_Window> initWindow(int width, int height)
{
    if (SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        std::cerr << "SDL could not intialize! SDL_error: " << SDL_GetError() << std::endl;
        return nullptr;
    }
    else
    {
        SDL_Window* window = SDL_CreateWindow( "SDL Tutorial",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            width,
            height,
            SDL_WINDOW_SHOWN );
        if ( NULL == window )
        {
            std::cerr << "Window could not be created! SDL_error: " << SDL_GetError() << std::endl;
            return nullptr;
        }

         return std::unique_ptr<SDL_Window>(window);
    }
}

std::unique_ptr<SDL_Renderer> initRenderer(const std::unique_ptr<SDL_Window>& window)
{
    SDL_Renderer *renderer = SDL_CreateRenderer( window.get(), -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
    if ( NULL == renderer )
    {
        std::cerr << "Renderer could not be created! SDL_error: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );

    return std::unique_ptr<SDL_Renderer>(renderer);
}

std::optional<Context> createContext(int widht, int height)
{
    auto window = initWindow(widht, height);
    if ( !window )
        return std::nullopt;

    auto renderer = initRenderer(window);
    if ( !renderer )
        return std::nullopt;

    int imgFlags = IMG_INIT_PNG;
    if ( !(IMG_Init( imgFlags ) & imgFlags ) )
    {
        std::cerr << "SDL_image could not be initialized! SDL_image Error: " << IMG_GetError() << std::endl;
        return std::nullopt;
    }

    if ( -1 == TTF_Init() )
    {
        std::cerr << "SDL_ttf could not be initialized! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return std::nullopt;
    }

    return Context(std::move(window), std::move(renderer));
}
