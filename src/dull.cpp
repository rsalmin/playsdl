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
