#include <iostream>

#include <SDL.h>

int main()
{
    //Screen dimension constants
    const int SCREEN_WIDTH = 640;
    const int SCREEN_HEIGHT = 480;

    SDL_Window* window = NULL;
    SDL_Surface* screenSurface = NULL;

    if (SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        std::cerr << "SDL could not intialize! SDL_error: " << SDL_GetError() << std::endl;
    }
    else
    {
        window = SDL_CreateWindow( "SDL Tutorial",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN );
        if ( NULL == window )
        {
            std::cerr << "Window could not be created! SDL_error: " << SDL_GetError() << std::endl;
        }
        else
        {
            screenSurface = SDL_GetWindowSurface( window );
            SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF ) );
            SDL_UpdateWindowSurface( window );

            SDL_Event e;
            bool quit = false;
            while ( !quit )
            {
               while ( SDL_PollEvent( &e ) )
               {
                  quit = SDL_QUIT == e.type ;
               }
            }
        }
        SDL_DestroyWindow(  window );
        SDL_Quit();
    }
}
