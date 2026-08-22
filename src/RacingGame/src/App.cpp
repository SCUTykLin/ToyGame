//
// Created by 15813 on 2026/8/17.
//

#include "App.h"
#include "game.h"
#include "SDL_Racing.h"

constexpr bool is_SDL = true;
constexpr int ScreenWidth = 1024;
constexpr int ScreenHeight = 720;

namespace Racing {
    void App::run(){
        if (is_SDL) SDL_gameLoop(1024, 720);
        else gameLoop();
    }
}