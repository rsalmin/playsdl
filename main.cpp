#include <iostream>
#include <memory>
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
class std::default_delete<SDL_Surface>
{
public:
    void operator()(SDL_Surface *ptr) const
    {
        SDL_FreeSurface( ptr );
    }
};




std::unique_ptr<SDL_Window> init()
{
    //Screen dimension constants
    const int SCREEN_WIDTH = 640;
    const int SCREEN_HEIGHT = 480;

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
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN );
        if ( NULL == window )
        {
            std::cerr << "Window could not be created! SDL_error: " << SDL_GetError() << std::endl;
            return nullptr;
        }
        return std::unique_ptr<SDL_Window>(window);
    }
}

std::unique_ptr<SDL_Surface> loadMedia()
{
  SDL_Surface* surface = SDL_LoadBMP("media/peace.bmp");
  if (NULL == surface)
  {
      std::cerr << "Unable to load image! SDL_error: " << SDL_GetError() << std::endl;
      return nullptr;
  }
  return std::unique_ptr<SDL_Surface>(surface);
}

int main()
{
    auto window = init();
    if ( !window )
        return -1;

    auto peaceImage = loadMedia();
    if ( !peaceImage )
        return -1;

    SDL_Surface* screenSurface = SDL_GetWindowSurface( window.get() );
    SDL_BlitSurface( peaceImage.get(), NULL, screenSurface, NULL );
    //SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF ) );
    SDL_UpdateWindowSurface( window.get() );

    SDL_Event e;
    bool quit = false;
    while ( !quit )
            {
               while ( SDL_PollEvent( &e ) )
               {
                  quit = SDL_QUIT == e.type ;
               }
            }

    SDL_Quit();
}
