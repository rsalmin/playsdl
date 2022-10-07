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
#include "surface.hpp"
#include "font.hpp"


class Media {
public:
   explicit Media(Texture&&, Texture&&, Texture&&, Texture&&);

   Texture m_mouseOutTexture;
   Texture m_mouseMotionTexture;
   Texture m_mouseButtonUpTexture;
   Texture m_mouseButtonDownTexture;
};

Media::Media(Texture&& outTexture, Texture&& motionTexture, Texture&& upTexture, Texture&& downTexture):
  m_mouseOutTexture(std::move(outTexture)) ,
  m_mouseMotionTexture(std::move(motionTexture)) ,
  m_mouseButtonUpTexture(std::move(upTexture)) ,
  m_mouseButtonDownTexture(std::move(downTexture))
{
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

    SDL_Event e;
    bool quit = false;

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

              for(auto& button : buttons)
                  button.handleEvent(e);

             SDL_RenderClear( context.renderer() );

             for(auto& button : buttons)
                 button.render(context, media);

            SDL_RenderPresent( context.renderer() );
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

    auto font = loadFont("media/lazy.ttf");
    if ( !font )
        return -1;

    SDL_Color textColor = {0, 0, 0};
    auto outTextureOpt = textureFromText(contextOpt.value(), "Mouse Out", font, textColor);
    if ( !outTextureOpt )
        return -1;
    auto motionTextureOpt = textureFromText(contextOpt.value(), "Mouse Motion", font, textColor);
    if ( !motionTextureOpt )
        return -1;
    auto upTextureOpt = textureFromText(contextOpt.value(), "Mouse Up", font, textColor);
    if ( !upTextureOpt )
        return -1;
    auto downTextureOpt = textureFromText(contextOpt.value(), "Mouse Down", font, textColor);
    if ( !downTextureOpt )
        return -1;

    Media media(std::move(outTextureOpt).value(), std::move(motionTextureOpt).value(),
        std::move(upTextureOpt).value(), std::move(downTextureOpt).value() );
    start( contextOpt.value(), media );

    SDL_Quit();
}
