//
// Created by 15813 on 2026/7/12.
//
#include "app.h"
#include "../Game/game.h"
#include "../TextureManager/textureManager.h"

namespace roguelike {
    void App::run() {

        constexpr  int FPS = 60;
        constexpr int frameDelay = 1000 / FPS;

        Uint32 frameStart;
        int frameTime;

        Game *game = nullptr;
        game = new Game();
        game->init("Birch Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, false);
        while (game->running()) {
            frameStart = SDL_GetTicks();
            game->handleEvents();
            game->update();
            game->render();
            frameTime = SDL_GetTicks() - frameStart;
            if (frameDelay > frameTime) {
                SDL_Delay(frameDelay - frameTime);
            }
        }
        game->clean();
    }

}