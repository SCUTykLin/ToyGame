//
// Created by 15813 on 2026/8/23.
//

#ifndef TOYPROJECTS_RACE_CAR_H
#define TOYPROJECTS_RACE_CAR_H

#include <vector>
#include <SDL2/SDL.h>

namespace Racing{
    typedef struct carGraph{

        typedef enum {
            NORMAL = 0,
            RIGHT,
            LEFT
        }CarStatus;

        struct CarRect{
            int x;
            int y;
            int w;
            int h;

            Uint8 r;
            Uint8 g;
            Uint8 b;
        };

        float width = 1, height = 1;

        std::vector<CarRect> normal_graph;
        std::vector<CarRect> right_graph;
        std::vector<CarRect> left_graph;
        carGraph();
        std::vector<carGraph::CarRect>& getCarGraph(CarStatus s);
        void render(float carX, float carY, float CELL, CarStatus status, SDL_Renderer* renderer);

    }carGraph_t;
}
#endif //TOYPROJECTS_RACE_CAR_H
