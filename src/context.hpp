#pragma once
#include <memory>
#include <optional>

#include <SDL.h>

template<>
class std::default_delete<SDL_Window>
{
public:
    void operator()(SDL_Window *ptr) const
    {
        SDL_DestroyWindow( ptr );
    }
};

template<>
class std::default_delete<SDL_Renderer>
{
public:
    void operator()(SDL_Renderer *ptr) const
    {
        SDL_DestroyRenderer( ptr );
    }
};

class Context
{
public:
    explicit Context(std::unique_ptr<SDL_Window>&& window, std::unique_ptr<SDL_Renderer>&& renderer):
        m_window(std::move(window)), m_renderer(std::move(renderer)) {}

    SDL_Renderer* renderer() noexcept { return m_renderer.get(); }

protected:
    std::unique_ptr<SDL_Window> m_window;
    std::unique_ptr<SDL_Renderer> m_renderer;
};


std::optional<Context> createContext(int widht, int height);
