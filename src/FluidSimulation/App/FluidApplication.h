//
// Created by 15813 on 2026/7/11.
//

#ifndef TOYPROJECTS_FLUIDAPPLICATION_H
#define TOYPROJECTS_FLUIDAPPLICATION_H
#include "../Grid/Grid.h"
#include "../Solver/FluidSolver.h"
#include "SDL2/SDL.h"

namespace fluid {

    class FluidApplication {
    public:
        void run();

    private:
        void process_events();

        void handle_mouse(int x, int y);

        void update();

        void render();

        Grid _grid;
        FluidSolver _fluidSolver;
        bool _running = true;
        SDL_Event event;
    };
}

#endif //TOYPROJECTS_FLUIDAPPLICATION_H
