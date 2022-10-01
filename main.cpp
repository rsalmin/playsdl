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
  SDL_Surface* surface = IMG_Load( path.c_str() );
  if (NULL == surface )
  {
      std::cerr << "Unable to load surface from " << path << "! IMG_error: " << IMG_GetError() << std::endl;
      return nullptr;
  }

  SDL_SetColorKey( surface, SDL_TRUE, SDL_MapRGB( surface->format, 0xFF, 0xFF, 0xFF ) );

  SDL_Texture* texture = SDL_CreateTextureFromSurface( renderer.get(), surface );
  if (NULL == texture )
  {
      std::cerr << "Unable to create texture from " << path << "! SDL_error: " << SDL_GetError() << std::endl;
      return nullptr;
  }

  SDL_FreeSurface( surface );

  return std::unique_ptr<SDL_Texture>(texture);
}

void renderGeometry(const std::unique_ptr<SDL_Renderer>& renderer, int width, int height)
{
    SDL_Rect fillRect = { width / 4, height / 4, width / 2, height / 2 };
    SDL_SetRenderDrawColor( renderer.get(), 0xFF, 0x00, 0x00, 0xFF );
    SDL_RenderFillRect( renderer.get(), &fillRect );

    SDL_Rect outlineRect = { width / 6, height / 6, width * 2 / 3, height * 2 / 3 };
    SDL_SetRenderDrawColor( renderer.get(), 0x00, 0xFF, 0x00, 0xFF );
    SDL_RenderDrawRect( renderer.get(), &outlineRect );

    SDL_SetRenderDrawColor( renderer.get(), 0xFF, 0xFF, 0x00, 0xFF );
    SDL_RenderDrawLine( renderer.get(), 0, height / 2, width, height / 2 );

    SDL_SetRenderDrawColor( renderer.get(), 0xFF, 0xFF, 0x00, 0xFF );
    for ( int i = 0; i < height; i += 4)
    {
        SDL_RenderDrawPoint( renderer.get(), width / 2, i );
    }
}

void renderBackground(const std::unique_ptr<SDL_Renderer>& renderer,
    const std::unique_ptr<SDL_Texture>& landscapeImage,
    const std::unique_ptr<SDL_Texture>& peaceImage,
    const SDL_Rect& wholeViewport,
    int rComponent, int gComponent, int bComponent)
{
    SDL_Rect topLeftViewport {
        .x = 0,
        .y = 0,
        .w = wholeViewport.w / 2,
        .h = wholeViewport.h  / 2
    };

    SDL_Rect bottomViewport {
        .x = 0,
        .y = wholeViewport.h  / 2,
        .w = wholeViewport.w,
        .h = wholeViewport.h  / 2
    };


    //SDL_SetRenderDrawColor( renderer.get(), 0x00, 0x00, 0xFF, 0xFF );
    //SDL_RenderClear( renderer.get() );
    SDL_RenderSetViewport( renderer.get(), &wholeViewport);
    SDL_SetTextureColorMod( landscapeImage.get(), rComponent, gComponent, bComponent );
    SDL_RenderCopy( renderer.get(), landscapeImage.get(), NULL, NULL );

    SDL_RenderSetViewport( renderer.get(), &topLeftViewport);
    SDL_RenderCopy( renderer.get(), peaceImage.get(), NULL, NULL );

    SDL_RenderSetViewport( renderer.get(), &bottomViewport);
    renderGeometry(renderer, bottomViewport.w, bottomViewport.h);
}

int main()
{
    //Screen dimension constants
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 960;

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

    auto landscapeImage = loadTexture("media/tree.png", renderer);
    if ( !landscapeImage )
        return -1;

    auto circlesImage = loadTexture("media/circles4.png", renderer);
    SDL_SetTextureBlendMode(circlesImage.get(), SDL_BLENDMODE_BLEND);
    if ( !circlesImage )
        return -1;

    auto walkingSprites = loadTexture("media/walkingSprites.png", renderer);
    if ( !walkingSprites )
        return -1;

    SDL_Rect wholeViewport {
        .x = 0,
        .y = 0,
        .w = SCREEN_WIDTH,
        .h = SCREEN_HEIGHT
    };

    enum class ArrowState { Up, Down, Left, Right, Default };

    std::array<SDL_Rect, 4> clips;
    for(int i = 0; i < 2; i++)
        for(int j = 0; j < 2; j++)
            clips[i*2 + j] = {.x = i*128, .y = j*128, .w = 128, .h = 128 };


    std::array<SDL_Rect, 4> spriteClips;
    spriteClips[0] = {.x = 10, .y = 0,  .w = 108, .h = 128 };
    spriteClips[1] = {.x = 138, .y = 0, .w = 108, .h = 128 };
    spriteClips[2] = {.x = 266, .y = 0, .w = 108, .h = 128 };
    spriteClips[3] = {.x = 394, .y = 0, .w = 108, .h = 128 };

    SDL_Rect renderQuad = { .x = 832, .y = 192, .w = 256, .h = 256 };

    SDL_Event e;
    std::uint32_t iFrame = 0;
    bool quit = false;
    std::uint8_t rComponent = 0xFF;
    std::uint8_t gComponent = 0xFF;
    std::uint8_t bComponent = 0xFF;
    std::uint8_t aComponent = 0xFF;
    ArrowState arrowState { ArrowState::Default };
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
                         case SDLK_w:
                             rComponent += 32;
                             std::cout << "Key: W! Exiting...." << std::endl;
                             break;
                         case SDLK_e:
                             gComponent += 32;
                             std::cout << "Key: E! Exiting...." << std::endl;
                             break;
                         case SDLK_r:
                             bComponent += 32;
                             std::cout << "Key: R! Exiting...." << std::endl;
                             break;
                         case SDLK_t:
                             aComponent += 32;
                             std::cout << "Key: T! Exiting...." << std::endl;
                             break;
                        case SDLK_s:
                             rComponent -= 32;
                             std::cout << "Key: S! Exiting...." << std::endl;
                             break;
                         case SDLK_d:
                             gComponent -= 32;
                             std::cout << "Key: D! Exiting...." << std::endl;
                             break;
                         case SDLK_f:
                             bComponent -= 32;
                             std::cout << "Key: F! Exiting...." << std::endl;
                             break;
                         case SDLK_g:
                             aComponent -= 32;
                             std::cout << "Key: G! Exiting...." << std::endl;
                             break;
                        case SDLK_q:
                             quit = true;
                             std::cout << "Key: Q! Exiting...." << std::endl;
                             break;
                         case SDLK_UP:
                             arrowState = ArrowState::Up;
                             std::cout << "Key: Up" << std::endl;
                             break;
                         case SDLK_DOWN:
                             arrowState = ArrowState::Down;
                             std::cout << "Key: Down" << std::endl;
                             break;
                         case SDLK_LEFT:
                             arrowState = ArrowState::Left;
                             std::cout << "Key: Left" << std::endl;
                             break;
                         case SDLK_RIGHT:
                             arrowState = ArrowState::Right;
                             std::cout << "Key: Right" << std::endl;
                             break;
                         default:
                             arrowState = ArrowState::Default;
                             std::cout << "Key: Any Other :)" << std::endl;
                             break;
                     }
                  }

            renderBackground(renderer, landscapeImage, peaceImage,  wholeViewport,
                                             rComponent, gComponent, bComponent );
            SDL_RenderSetViewport( renderer.get(), &wholeViewport);

            switch (arrowState)
            {
                case ArrowState::Up:
                    SDL_RenderCopy( renderer.get(), upImage.get(), NULL, &renderQuad);
                    break;
                case ArrowState::Down:
                    SDL_RenderCopyEx( renderer.get(), upImage.get(), NULL, &renderQuad, 180, NULL, SDL_FLIP_NONE);
                    break;
                case ArrowState::Left:
                    SDL_RenderCopyEx( renderer.get(), upImage.get(), NULL, &renderQuad, 270, NULL, SDL_FLIP_NONE);
                    break;
                 case ArrowState::Right:
                    SDL_RenderCopyEx( renderer.get(), upImage.get(), NULL, &renderQuad, 90, NULL, SDL_FLIP_NONE);
                    break;
                case ArrowState::Default:
                    SDL_RenderCopy( renderer.get(), defaultImage.get(), NULL, &renderQuad);
                    break;
           }

            std::array<SDL_Rect, 4> rects;
            for(int i = 0; i < 2; i++)
                for(int j = 0; j < 2; j++)
                    rects[i*2 +j] = { .x = 160 + 320*(i*2 + j), .y = 820, .w = 128, .h = 128 };

            SDL_SetTextureAlphaMod( circlesImage.get(), aComponent );
            for(int i = 0; i < 4; i++)
                SDL_RenderCopy( renderer.get(), circlesImage.get(), &clips[i], &rects[i]);

           SDL_RenderSetViewport( renderer.get(), &wholeViewport);
            const auto iClip = ( iFrame / 4 ) % 4;
            SDL_Rect walkingRect = {.x = SCREEN_WIDTH / 2 - 64, .y = SCREEN_HEIGHT / 2 - 64, .w = 128, .h = 128};
            SDL_RenderCopy( renderer.get(), walkingSprites.get(), &spriteClips[iClip], &walkingRect );

            SDL_RenderPresent( renderer.get() );
            iFrame++;
        }
    }

    SDL_Quit();
}
