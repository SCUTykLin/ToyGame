//
// Created by YangKai.Lin on 2026/7/26.
//

#include "App.h"
#include "game.h"
#include "game_sdl.h"

namespace Tetris {
    bool App::is_console = false;
    void App::run() {
        if (is_console) {
            std::cout << "The Console version should be ran on external console,"
                         "and RESET THE (80, 30) SIZE" << std::endl;
            mainLoop();
        }else {
            SDL_Version::mainLoop();
        }
    }
}
