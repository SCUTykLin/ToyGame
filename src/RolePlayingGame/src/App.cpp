//
// Created by YangKai.Lin on 2026/8/30.
//

#include "App.h"
#include "RPG_Engine.h"

namespace RoleGame {
    void App::run() {
        RPG_Engine game;
        if (game.ConstructConsole(256, 240, 4, 4)) {
            game.Start();
        }
    }
}
