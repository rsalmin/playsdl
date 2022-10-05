#include <iostream>
#include <memory>
#include <filesystem>

#include <SDL.h>
#include <SDL_image.h>

#include "surface.hpp"

std::unique_ptr<SDL_Surface> loadSurface(const std::filesystem::path& path, const SDL_PixelFormat* pixelFormat)
{
  SDL_Surface* surface = IMG_Load( path.c_str() );
  if (NULL == surface)
  {
      std::cerr << "Unable to load image! IMG_error: " << IMG_GetError() << std::endl;
      return nullptr;
  }

  SDL_Surface* optimizedSurface = SDL_ConvertSurface( surface, pixelFormat,  0);
  if (NULL == optimizedSurface)
  {
      std::cerr << "Unable to optimize image! SDL_error: " << SDL_GetError() << std::endl;
      return nullptr;
  }

  SDL_FreeSurface(surface);
  return std::unique_ptr<SDL_Surface>(optimizedSurface);
}
