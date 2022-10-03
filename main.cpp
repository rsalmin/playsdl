#include <iostream>
#include <memory>
#include <filesystem>
#include <optional>
#include <cassert>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

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

template<>
class std::default_delete<TTF_Font>
{
public:
    void operator()(TTF_Font *ptr) const
    {
        TTF_CloseFont( ptr );
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

class Texture
{
public:
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

protected:
    std::unique_ptr<SDL_Texture> m_texture;
    int m_w {0};
    int m_h {0};
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

std::unique_ptr<TTF_Font> loadFont(const std::filesystem::path& path)
{
    TTF_Font *font = TTF_OpenFont( path.c_str(), 28);
    if ( NULL == font )
    {
        std::cerr << "Failed to load lazy font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return nullptr;
    }

    return std::unique_ptr<TTF_Font>(font);
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

void renderGeometry(Context& ctx, int width, int height)
{
    SDL_Rect fillRect = { width / 4, height / 4, width / 2, height / 2 };
    SDL_SetRenderDrawColor( ctx.renderer(), 0xFF, 0x00, 0x00, 0xFF );
    SDL_RenderFillRect( ctx.renderer(), &fillRect );

    SDL_Rect outlineRect = { width / 6, height / 6, width * 2 / 3, height * 2 / 3 };
    SDL_SetRenderDrawColor( ctx.renderer(), 0x00, 0xFF, 0x00, 0xFF );
    SDL_RenderDrawRect( ctx.renderer(), &outlineRect );

    SDL_SetRenderDrawColor( ctx.renderer(), 0xFF, 0xFF, 0x00, 0xFF );
    SDL_RenderDrawLine( ctx.renderer(), 0, height / 2, width, height / 2 );

    SDL_SetRenderDrawColor( ctx.renderer(), 0xFF, 0xFF, 0x00, 0xFF );
    for ( int i = 0; i < height; i += 4)
    {
        SDL_RenderDrawPoint( ctx.renderer(), width / 2, i );
    }
}

void renderBackground(Context& ctx,
    Texture& landscapeImage,
    Texture& peaceImage,
    const SDL_Rect& wholeViewport,
    std::uint8_t rComponent, std::uint8_t gComponent, std::uint8_t bComponent)
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
    SDL_RenderSetViewport( ctx.renderer(), &wholeViewport);
    landscapeImage.setColorMod( rComponent, gComponent, bComponent );
    SDL_RenderCopy( ctx.renderer(), landscapeImage.texture(), NULL, NULL );

    SDL_RenderSetViewport( ctx.renderer(), &topLeftViewport);
    SDL_RenderCopy( ctx.renderer(), peaceImage.texture(), NULL, NULL );

    SDL_RenderSetViewport( ctx.renderer(), &bottomViewport);
    renderGeometry( ctx, bottomViewport.w, bottomViewport.h);
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

int main()
{
    //Screen dimension constants
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 960;

    auto contextOpt = createContext(SCREEN_WIDTH, SCREEN_HEIGHT);
    if ( !contextOpt )
        return -1;
    auto context = std::move(contextOpt).value();

    auto peaceImageOpt = loadTexture("media/peace.png", context);
    if ( !peaceImageOpt )
        return -1;
    auto peaceImage = std::move(peaceImageOpt).value();

    auto upImageOpt = loadTexture("media/up.png", context);
    if ( !upImageOpt )
        return -1;
    auto upImage = std::move(upImageOpt).value();

    auto defaultImageOpt = loadTexture("media/default.png", context);
    if ( !defaultImageOpt )
        return -1;
    auto defaultImage = std::move(defaultImageOpt).value();

    auto landscapeImageOpt = loadTexture("media/tree.png", context);
    if ( !landscapeImageOpt )
        return -1;
    auto landscapeImage = std::move(landscapeImageOpt).value();

    auto circlesImageOpt = loadTexture("media/circles4.png", context);
    if ( !circlesImageOpt )
        return -1;
    auto circlesImage = std::move(circlesImageOpt).value();
    circlesImage.setBlendMode(SDL_BLENDMODE_BLEND);

    auto walkingSpritesOpt = loadTexture("media/walkingSprites.png", context);
    if ( !walkingSpritesOpt )
        return -1;
    auto walkingSprites = std::move(walkingSpritesOpt).value();

    auto font = loadFont("media/lazy.ttf");
    if ( !font )
        return -1;

    SDL_Color textColor = {0, 0, 0};
    auto textTextureOpt = textureFromText(context, "The quick brown fox jumps over the lazy dogs, ну типа..", font, textColor);
    if ( !textTextureOpt )
        return -1;
    auto textTexture =std::move(textTextureOpt).value();

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

            renderBackground(context, landscapeImage, peaceImage,  wholeViewport,
                                             rComponent, gComponent, bComponent );
            SDL_RenderSetViewport( context.renderer(), &wholeViewport);

            switch (arrowState)
            {
                case ArrowState::Up:
                    SDL_RenderCopy( context.renderer(), upImage.texture(), NULL, &renderQuad);
                    break;
                case ArrowState::Down:
                    SDL_RenderCopyEx( context.renderer(), upImage.texture(), NULL, &renderQuad, 180, NULL, SDL_FLIP_NONE);
                    break;
                case ArrowState::Left:
                    SDL_RenderCopyEx( context.renderer(), upImage.texture(), NULL, &renderQuad, 270, NULL, SDL_FLIP_NONE);
                    break;
                 case ArrowState::Right:
                    SDL_RenderCopyEx( context.renderer(), upImage.texture(), NULL, &renderQuad, 90, NULL, SDL_FLIP_NONE);
                    break;
                case ArrowState::Default:
                    SDL_RenderCopy( context.renderer(), defaultImage.texture(), NULL, &renderQuad);
                    break;
           }

            std::array<SDL_Rect, 4> rects;
            for(int i = 0; i < 2; i++)
                for(int j = 0; j < 2; j++)
                    rects[i*2 +j] = { .x = 160 + 320*(i*2 + j), .y = 820, .w = 128, .h = 128 };

            SDL_SetTextureAlphaMod( circlesImage.texture(), aComponent );
            for(int i = 0; i < 4; i++)
                SDL_RenderCopy( context.renderer(), circlesImage.texture(), &clips[i], &rects[i]);

           SDL_RenderSetViewport( context.renderer(), &wholeViewport);
            const auto iClip = ( iFrame / 4 ) % 4;
            SDL_Rect walkingRect = {.x = SCREEN_WIDTH / 2 - 64, .y = SCREEN_HEIGHT / 2 - 64, .w = 128, .h = 128};
            SDL_RenderCopy( context.renderer(), walkingSprites.texture(), &spriteClips[iClip], &walkingRect );

            SDL_Rect rText { .x = ( SCREEN_WIDTH - textTexture.width() ) / 2,
                                         .y = (SCREEN_HEIGHT - textTexture.height() ) / 2,
                                         .w = textTexture.width(),
                                         .h = textTexture.height()
                                       };
            SDL_RenderCopy( context.renderer(), textTexture.texture(), NULL, &rText);

            SDL_RenderPresent( context.renderer() );
            iFrame++;
        }
    }

    SDL_Quit();
}
