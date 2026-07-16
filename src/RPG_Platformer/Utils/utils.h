//
// Created by YangKai.Lin on 2026/7/16.
//

#ifndef TOYPROJECTS_UTILS_H
#define TOYPROJECTS_UTILS_H
#include <SDL_timer.h>

namespace RPG_Platformer::Utils {
    inline float hireTimeInSeconds() {
        float t = SDL_GetTicks();
        t *= 0.001f;
        return t;
    }
}

#endif //TOYPROJECTS_UTILS_H