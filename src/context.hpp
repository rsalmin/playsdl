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
    explicit Context(std::unique_ptr<SDL_Window>&& window, std::unique_ptr<SDL_Renderer>&& renderer,
        int width, int height):
        m_window(std::move(window)), m_renderer(std::move(renderer)),
        m_width(width), m_height(height) {}

    SDL_Renderer* renderer() noexcept { return m_renderer.get(); }

    int width() const noexcept {return m_width;}
    int height() const noexcept {return m_height;}

protected:
    std::unique_ptr<SDL_Window> m_window;
    std::unique_ptr<SDL_Renderer> m_renderer;
    int m_width{0};
    int m_height{0};
};


std::optional<Context> createContext(int widht, int height);
