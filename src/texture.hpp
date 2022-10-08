#include <iostream>
#include <memory>
#include <filesystem>
#include <optional>
#include <cassert>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "context.hpp"

template<>
class std::default_delete<SDL_Texture>
{
public:
    void operator()(SDL_Texture *ptr) const
    {
        SDL_DestroyTexture( ptr );
    }
};

class Texture
{
public:
    Texture() = delete;

    Texture(SDL_Texture* texture, int w, int h): m_texture(texture), m_w(w), m_h(h)
    {
        assert( texture );
    }

    void setBlendMode(const SDL_BlendMode& blendMode)
    {
        SDL_SetTextureBlendMode(m_texture.get(), blendMode);
    }

    void setColorMod(const std::uint8_t r, std::uint8_t g, std::uint8_t b)
    {
        SDL_SetTextureColorMod( m_texture.get(), r, g, b );
    }

    SDL_Texture* texture() noexcept { return m_texture.get(); }

    int width() const noexcept {return m_w;}
    int height() const noexcept {return m_h;}

    void renderAt(Context& , int x, int y);

protected:
    std::unique_ptr<SDL_Texture> m_texture;
    int m_w {0};
    int m_h {0};
};

std::optional<Texture> loadTexture(const std::filesystem::path& path, Context& ctx);

std::optional<Texture> textureFromText(Context& ctx, const std::string& text,  const std::unique_ptr<TTF_Font>& font, const SDL_Color& color);
