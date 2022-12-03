#include <iostream>
#include <iomanip>
#include <memory>
#include <filesystem>
#include <optional>
#include <cassert>
#include <chrono>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include "context.hpp"
#include "texture.hpp"
#include "surface.hpp"
#include "font.hpp"
#include "music.hpp"
#include "fps_counter.hpp"
#include "ball.hpp"
#include "scene.hpp"

class TextMaker
{
public:
  TextMaker(Font&& font): m_font(std::move(font)) { assert(m_font);}

  std::optional<Texture> fromString(Context& ctx, const std::string& str)
  {
     return textureFromText(ctx, str.c_str(), m_font, m_textColor);
  }
private:
  Font m_font;
  SDL_Color m_textColor {0, 0, 0};
};

class Media {
public:
   Media() = delete;
   explicit Media(Texture&&, Texture&&, Texture&&, Texture&&, Texture&& , Texture&&,
                 MixMusic&&, MixChunk&&, MixChunk&&, MixChunk&&, MixChunk&&, Texture&&,
                 TextMaker&&);

   Texture m_mouseOutTexture;
   Texture m_mouseMotionTexture;
   Texture m_mouseButtonUpTexture;
   Texture m_mouseButtonDownTexture;

   SDL_Texture* arrowTexture() noexcept {return m_arrowTexture.texture();}
   SDL_Texture* defaultTexture() noexcept {return m_defaultTexture.texture();}

   Texture& info() noexcept {return m_info;}

   Mix_Music* music() noexcept {return m_music.get();}
   Mix_Chunk* scratchChunk() noexcept {return m_scratchChunk.get();}
   Mix_Chunk* lowChunk() noexcept {return m_lowChunk.get();}
   Mix_Chunk* mediumChunk() noexcept {return m_mediumChunk.get();}
   Mix_Chunk* highChunk() noexcept {return m_highChunk.get();}

   void updateInfo(Context& ctx, const std::string& str);
protected:
    Texture m_arrowTexture;
    Texture m_defaultTexture;

    MixMusic m_music;

    MixChunk m_scratchChunk;
    MixChunk m_lowChunk;
    MixChunk m_mediumChunk;
    MixChunk m_highChunk;

    Texture m_info;

    TextMaker m_textMaker;
};

Media::Media(
    Texture&& outTexture, Texture&& motionTexture, Texture&& upTexture, Texture&& downTexture,
    Texture&& arrowTexture, Texture&& defaultTexture,
    MixMusic&& music, MixChunk&& scratchChunk, MixChunk&& lowChunk,
    MixChunk&& mediumChunk, MixChunk&& highChunk, Texture&& info,
    TextMaker&& textMaker
):
  m_mouseOutTexture(std::move(outTexture)) ,
  m_mouseMotionTexture(std::move(motionTexture)) ,
  m_mouseButtonUpTexture(std::move(upTexture)) ,
  m_mouseButtonDownTexture(std::move(downTexture)),
  m_arrowTexture(std::move(arrowTexture)),
  m_defaultTexture(std::move(defaultTexture)),
  m_music(std::move(music)),
  m_scratchChunk(std::move(scratchChunk)),
  m_lowChunk(std::move(lowChunk)),
  m_mediumChunk(std::move(mediumChunk)),
  m_highChunk(std::move(highChunk)),
  m_info(std::move(info)),
  m_textMaker(std::move(textMaker))
{
}

void Media::updateInfo(Context& ctx, const std::string& str)
{
    auto infoOpt = m_textMaker.fromString(ctx, str);
    if (infoOpt)
        m_info = std::move(infoOpt).value();
}

class Button {
public:
  enum class MouseEventType { Out, Motion, Down, Up };

  explicit Button(const SDL_Rect& bounds): m_bounds(bounds) {}

  void handleEvent( const SDL_Event& );

  void render(Context& ctx, Media& media);

protected:
  MouseEventType m_eventType { MouseEventType::Out };
  SDL_Rect m_bounds {.x = 0, .y = 0, .w = 0, .h = 0};
};

void Button::handleEvent( const SDL_Event& e )
{
    if (SDL_MOUSEMOTION != e.type && SDL_MOUSEBUTTONDOWN != e.type && SDL_MOUSEBUTTONUP != e.type)
        return;

    int x{0}, y{0};
    SDL_GetMouseState(&x, &y);

    const bool isInside =
        x >= m_bounds.x &&
        x < m_bounds.x + m_bounds.w &&
        y >= m_bounds.y &&
        y <= m_bounds.y + m_bounds.h;

    if (! isInside)
        m_eventType = MouseEventType::Out;
    else
        switch (e.type) {
            case SDL_MOUSEMOTION: m_eventType = MouseEventType::Motion; break ;
            case SDL_MOUSEBUTTONDOWN: m_eventType = MouseEventType::Down; break;
            case SDL_MOUSEBUTTONUP: m_eventType = MouseEventType::Up; break;
        }
}

void Button::render(Context& ctx, Media& media)
{
    Texture* pointer { nullptr };

    switch (m_eventType) {
        case MouseEventType::Out: pointer = &media.m_mouseOutTexture; break;
        case MouseEventType::Motion: pointer = &media.m_mouseMotionTexture; break;
        case MouseEventType::Up: pointer = &media.m_mouseButtonUpTexture; break;
        case MouseEventType::Down: pointer = &media.m_mouseButtonDownTexture; break;
    }

    if (nullptr == pointer)
    {
        std::cerr << "Can't choose texture in Button::render " << std::endl;
        return;
    }

    SDL_SetRenderDrawColor( ctx.renderer(), 0xFF, 0xFF, 0x11, 0xFF );
    SDL_RenderFillRect( ctx.renderer(), &m_bounds);

    const int w = pointer->width();
    const int h = pointer->height();
    const int x = m_bounds.x + ( m_bounds.w -  w ) / 2;
    const int y = m_bounds.y + ( m_bounds.h -  h ) / 2;
    SDL_Rect textPosition { .x = x, .y = y, .w = w, .h = h };

    SDL_RenderCopy( ctx.renderer(), pointer->texture(), NULL, &textPosition );
}

class Arrow {
public:
  enum class ArrowState { Left, Up, Right, Down, Default };

  explicit Arrow(const SDL_Rect& bounds): m_bounds(bounds) {}

  void setState( ArrowState  state) noexcept { m_arrowState = state; }

  void render(Context& ctx, Media& media);

protected:
  ArrowState m_arrowState { ArrowState::Default };
  SDL_Rect m_bounds {.x = 0, .y = 0, .w = 0, .h = 0};
};

void Arrow::render(Context& ctx, Media& media)
{
    SDL_Texture* arrowTexture = media.arrowTexture();
    SDL_Texture* defaultTexture = media.defaultTexture();

    switch (m_arrowState) {
        case ArrowState::Left:
            SDL_RenderCopyEx( ctx.renderer(), arrowTexture, NULL, &m_bounds, 270, NULL, SDL_FLIP_NONE );
            break;
        case ArrowState::Up:
            SDL_RenderCopy( ctx.renderer(), arrowTexture, NULL, &m_bounds );
            break;
        case ArrowState::Right:
            SDL_RenderCopyEx( ctx.renderer(), arrowTexture, NULL, &m_bounds, 90, NULL, SDL_FLIP_NONE );
            break;
        case ArrowState::Down:
            SDL_RenderCopyEx( ctx.renderer(), arrowTexture, NULL, &m_bounds, 180, NULL, SDL_FLIP_NONE );
            break;
        case ArrowState::Default:
            SDL_RenderCopy( ctx.renderer(), defaultTexture, NULL, &m_bounds);
            break;
    };
}

void start(Context& context, Media& media)
{
    const int w2 = context.width() / 2;
    const int h2 = context.height() / 2;

    std::array<Button, 4> buttons = {
        Button({.x =  0, .y =  0, .w = w2, .h = h2}) ,
        Button({.x = w2, .y =  0, .w = w2, .h = h2}) ,
        Button({.x =  0, .y = h2, .w = w2, .h = h2}) ,
        Button({.x = w2, .y = h2, .w = w2, .h = h2})
    };

    Arrow arrow({.x = w2 - 100, .y = h2 - 100, .w = 200, .h = 200});

    Scene scene;

    SDL_Event e;
    bool quit = false;

    std::uint32_t lastFPS10 = 0;
    FPSCounter fpsCounter;

    std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();

    while ( !quit )
    {
        while ( SDL_PollEvent( &e ) )
         {
              if (   SDL_QUIT == e.type
                  || (SDL_KEYDOWN == e.type &&  SDLK_q == e.key.keysym.sym)
                  )
              {
                  quit = true;
              }

            if (SDL_KEYDOWN == e.type)
            {
                switch (e.key.keysym.sym)
                {
                    case SDLK_1:
                        Mix_PlayChannel(-1, media.highChunk(), 0);
                        break;
                    case SDLK_2:
                        Mix_PlayChannel(-1, media.mediumChunk(), 0);
                        break;
                    case SDLK_3:
                        Mix_PlayChannel(-1, media.lowChunk(), 0);
                        break;
                    case SDLK_4:
                        Mix_PlayChannel(-1, media.scratchChunk(), 0);
                        break;
                    case SDLK_9:
                        if ( 0 == Mix_PlayingMusic() )
                        {
                            Mix_PlayMusic( media.music(), -1);
                        }
                        else
                        {
                            if (1 == Mix_PausedMusic())
                            {
                                Mix_ResumeMusic();
                            }
                            else
                            {
                                Mix_PauseMusic();
                            }
                        }
                        break;
                    case SDLK_0:
                        Mix_HaltMusic();
                        break;
                };
            }

              for(auto& button : buttons)
                  button.handleEvent(e);
        }

       const std::uint8_t* currentKeyStates = SDL_GetKeyboardState( NULL );
       if (currentKeyStates[ SDL_SCANCODE_UP ] )
       {
           arrow.setState( Arrow::ArrowState::Up );
       }
       else if (currentKeyStates[ SDL_SCANCODE_DOWN ] )
       {
           arrow.setState( Arrow::ArrowState::Down );
       }
       else if (currentKeyStates[ SDL_SCANCODE_LEFT ] )
       {
           arrow.setState( Arrow::ArrowState::Left );
       }
       else if (currentKeyStates[ SDL_SCANCODE_RIGHT ] )
       {
           arrow.setState( Arrow::ArrowState::Right );
       }
       else
       {
           arrow.setState( Arrow::ArrowState::Default );
       }

       // Unpdate scene
       const auto now = std::chrono::steady_clock::now();
       scene.update(context, now - lastTime);
       lastTime = now;

        // Let's Render
        SDL_RenderClear( context.renderer() );
        for(auto& button : buttons)
            button.render(context, media);
        media.info().renderAt(context, w2 - media.info().width()/2, 50);
        arrow.render(context, media);
        scene.render(context);
        SDL_RenderPresent( context.renderer() );

       ++fpsCounter;

       const std::uint32_t fps10 = fpsCounter.fps10();
       if (lastFPS10 != fps10)
       {
           std::stringstream str;
           str << "fps : " << std::fixed << std::setprecision(1) << (fps10 / 10.0);
           media.updateInfo(context, str.str());
           lastFPS10 = fps10;
       }
    }
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

    auto font = loadFont("media/lazy.ttf");
    if ( !font )
        return -1;
    auto textMaker = TextMaker(std::move(font));

    auto outTextureOpt = textMaker.fromString(context, "Mouse Out");
    if ( !outTextureOpt )
        return -1;
    auto motionTextureOpt = textMaker.fromString(context, "Mouse Motion");
    if ( !motionTextureOpt )
        return -1;
    auto upTextureOpt = textMaker.fromString(context, "Mouse Up");
    if ( !upTextureOpt )
        return -1;

    auto downTextureOpt = textMaker.fromString(context, "Mouse Down");
    if ( !downTextureOpt )
        return -1;

    auto arrowImageOpt = loadTexture("media/up.png", context);
    if ( !arrowImageOpt )
        return -1;

    auto defaultImageOpt = loadTexture("media/default.png", context);
    if ( !defaultImageOpt )
        return -1;

    auto music = loadMusic("media/beat.wav");
    if (! music)
        return -1;
    auto scratch = loadChunk("media/scratch.wav");
    if (! scratch)
        return -1;
    auto high = loadChunk("media/high.wav");
    if (! high)
        return -1;
    auto medium = loadChunk("media/medium.wav");
    if (! medium)
        return -1;
    auto low = loadChunk("media/low.wav");
    if (! low)
        return -1;

    std::stringstream str;
    str << "Milliseconds for initalizing and load media : " << SDL_GetTicks();
    auto infoOpt = textMaker.fromString(context,   str.str());
    if (! infoOpt)
        return -1;

    Media media(std::move(outTextureOpt).value(), std::move(motionTextureOpt).value(),
        std::move(upTextureOpt).value(), std::move(downTextureOpt).value(),
        std::move(arrowImageOpt).value(), std::move(defaultImageOpt).value(),
        std::move(music), std::move(scratch), std::move(low),
        std::move(medium), std::move(high), std::move(infoOpt).value(),
        std::move(textMaker) );

    start( context, media );

    SDL_Quit();
}
