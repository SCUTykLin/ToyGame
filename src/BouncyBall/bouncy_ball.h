//
// Created by YangKai.Lin on 2026/7/7.
//

#ifndef TOYPROJECTS_BOUNCYBALL_BOUNCY_BALL_H
#define TOYPROJECTS_BOUNCYBALL_BOUNCY_BALL_H

#include <SDL2/SDL.h>
#include "window_tool.h"

#define WIDTH 900
#define HEIGHT 600
#define GRAVITY 2

namespace bouncy {

    namespace window_tool{
        struct SDL_Circle;
    }

    typedef struct Physic_Circle {
        int x;
        int y;
        int radius;
        int v_x;
        int v_y;
        window_tool::SDL_Circle* sdl_circle;
        Physic_Circle(int x, int y, int radius, int v_x, int v_y);
        ~Physic_Circle();
        void step();
    }Physic_Circle;

    void start_bouncy();
    void main_loop();
}

#endif //TOYPROJECTS_BOUNCYBALL_BOUNCY_BALL_H