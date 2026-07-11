//
// Created by 15813 on 2026/7/12.
//

#ifndef TOYPROJECTS_ROGUELIKE_APP_H
#define TOYPROJECTS_ROGUELIKE_APP_H

#include "../Renderer/renderer.h"
#include "SDL2/SDL.h"

#define W 800
#define H 600

namespace roguelike {
    class App {
    public:
        void start();
    private:
        Renderer renderer;
    };
}


#endif //TOYPROJECTS_ROGUELIKE_APP_H
