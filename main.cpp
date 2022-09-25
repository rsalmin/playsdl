#include <iostream>
#include <memory>
#include <filesystem>

#include <SDL.h>
#include <SDL_image.h>

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
class std::default_delete<SDL_Surface>
{
public:
    void operator()(SDL_Surface *ptr) const
    {
        SDL_FreeSurface( ptr );
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

template<>
class std::default_delete<SDL_Texture>
{
public:
    void operator()(SDL_Texture *ptr) const
    {
        SDL_DestroyTexture( ptr );
    }
};


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
    SDL_Renderer *renderer = SDL_CreateRenderer( window.get(), -1, SDL_RENDERER_ACCELERATED );
    if ( NULL == renderer )
    {
        std::cerr << "Renderer could not be created! SDL_error: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );

    return std::unique_ptr<SDL_Renderer>(renderer);
}

bool initSDLImage()
{
    int imgFlags = IMG_INIT_PNG;
    if ( !(IMG_Init( imgFlags ) & imgFlags ) )
    {
        std::cerr << "SDL_image could not be initialized! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }
    return true;
}

std::unique_ptr<SDL_Surface> loadSurface(const std::filesystem::path& path, const SDL_PixelFormat* pixelFormat)
{
  SDL_Surface* surface = IMG_Load( path.c_str() );
  if (NULL == surface)
  {
      std::cerr << "Unable to load image! SDL_error: " << SDL_GetError() << std::endl;
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

std::unique_ptr<SDL_Texture> loadTexture(const std::filesystem::path& path, const std::unique_ptr<SDL_Renderer>& renderer)
{
  SDL_Texture* texture = IMG_LoadTexture( renderer.get(), path.c_str() );
  if (NULL == texture )
  {
      std::cerr << "Unable to load texture from " << path << "! SDL_error: " << SDL_GetError() << std::endl;
      return nullptr;
  }

  return std::unique_ptr<SDL_Texture>(texture);
}

int main()
{
    //Screen dimension constants
    const int SCREEN_WIDTH = 640;
    const int SCREEN_HEIGHT = 480;

    auto window = initWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
    if ( !window )
        return -1;

    auto renderer = initRenderer(window);
    if ( !renderer )
        return -1;

    if ( ! initSDLImage() )
        return -1;

    SDL_Surface* screenSurface = SDL_GetWindowSurface( window.get() );

    auto peaceImage = loadTexture("media/peace.png", renderer);
    if ( !peaceImage )
        return -1;

    auto upImage = loadTexture("media/up.png", renderer);
    if ( !upImage )
        return -1;

    auto defaultImage = loadTexture("media/default.png", renderer);
    if ( !defaultImage )
        return -1;

    SDL_Rect stretchRect;
    stretchRect.x = 0;
    stretchRect.y = 0;
    stretchRect.w = SCREEN_WIDTH;
    stretchRect.h = SCREEN_HEIGHT;

    SDL_Event e;
    bool quit = false;
    SDL_Texture *currentTexture = peaceImage.get();
    while ( !quit )
    {
        while ( SDL_PollEvent( &e ) )
         {
              if ( SDL_QUIT == e.type )
              {
                  quit = true;
              }
              else if (SDL_KEYDOWN == e.type)
                  {
                     switch (e.key.keysym.sym)
                     {
                         case SDLK_q:
                             quit = true;
                             std::cout << "Key: Q! Exiting...." << std::endl;
                             break;
                         case SDLK_UP:
                             currentTexture = upImage.get();
                             std::cout << "Key: Up" << std::endl;
                             break;
                         case SDLK_DOWN:
                             currentTexture = upImage.get();
                             std::cout << "Key: Down" << std::endl;
                             break;
                         case SDLK_LEFT:
                             currentTexture = upImage.get();
                             std::cout << "Key: Left" << std::endl;
                             break;
                         case SDLK_RIGHT:
                             currentTexture = upImage.get();
                             std::cout << "Key: Right" << std::endl;
                             break;
                         default:
                             currentTexture = defaultImage.get();
                             std::cout << "Key: Any Other :)" << std::endl;
                             break;
                     }
                  }
        SDL_RenderClear( renderer.get() );
        SDL_RenderCopy( renderer.get(), currentTexture, NULL, NULL );
        SDL_RenderPresent( renderer.get() );
        }
    }

    SDL_Quit();
}
