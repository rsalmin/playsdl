#include <iostream>
#include <memory>
#include <filesystem>
#include <optional>
#include <cassert>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "context.hpp"
#include "texture.hpp"

std::optional<Texture> loadTexture(const std::filesystem::path& path, Context& ctx)
{
  SDL_Surface* surface = IMG_Load( path.c_str() );
  if (NULL == surface )
  {
      std::cerr << "Unable to load surface from " << path << "! IMG_error: " << IMG_GetError() << std::endl;
      return std::nullopt;
  }

  SDL_SetColorKey( surface, SDL_TRUE, SDL_MapRGB( surface->format, 0xFF, 0xFF, 0xFF ) );

  SDL_Texture* texture = SDL_CreateTextureFromSurface( ctx.renderer(), surface );
  if (NULL == texture )
  {
      std::cerr << "Unable to create texture from " << path << "! SDL_error: " << SDL_GetError() << std::endl;
      return std::nullopt;
  }

  Texture r(texture, surface->w, surface->h);

  SDL_FreeSurface( surface );

  return r;
}

std::optional<Texture> textureFromText(Context& ctx, const std::string& text,  const std::unique_ptr<TTF_Font>& font, const SDL_Color& color)
{
  SDL_Surface* surface = TTF_RenderText_Solid( font.get(), text.c_str(), color );
  if (NULL == surface )
  {
      std::cerr << "Unable to render text from " << text << "! SDL_ttf Error: " << TTF_GetError() << std::endl;
      return std::nullopt;
  }

  SDL_Texture* texture = SDL_CreateTextureFromSurface( ctx.renderer(), surface );
  const int w = surface->w;
  const int h = surface->h;
  SDL_FreeSurface( surface );

  if (NULL == texture )
  {
      std::cerr << "Unable to create texture from sufrace ! SDL_error: " << SDL_GetError() << std::endl;
      return std::nullopt;
  }

  return Texture(texture, w, h);
}
