//
// Created by 15813 on 2026/6/30.
//

#ifndef TOYPROJECTS_PONG_GAME_H
#define TOYPROJECTS_PONG_GAME_H

#include <iostream>
#include <SDL.h>
#include <SDL_main.h>

namespace Pong {
    constexpr static Uint32 black_color = 0x00000000;
    constexpr static Uint32 white_color = 0xffffffff;
    constexpr static int MOVEMENT_SPEED = 10;

    void startPongGame();

    void main_loop();
}
#endif //TOYPROJECTS_PONG_GAME_H
