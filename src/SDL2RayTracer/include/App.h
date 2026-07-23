//
// Created by YangKai.Lin on 2026/7/19.
//

#ifndef TOYPROJECTS_SDL_RAYTRACING_APP_H
#define TOYPROJECTS_SDL_RAYTRACING_APP_H

#include "CApp.h"

namespace RayTracing {
    class App {
    public:
        inline void run() {
            CApp{}.OnExecute();
        }
    };
}


#endif //TOYPROJECTS_SDL_RAYTRACING_APP_H