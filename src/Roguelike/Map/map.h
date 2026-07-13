//
// Created by YangKai.Lin on 2026/7/13.
//

#ifndef TOYPROJECTS_ROGUELIKE_MAP_H
#define TOYPROJECTS_ROGUELIKE_MAP_H
#include "../Game/game.h"
namespace roguelike {
    class Map {
    public:
        Map();
        ~Map();

        void LoadMap();
        void DrawMap();
    private:
        SDL_Rect src, dest;
        SDL_Texture *dirt;
        SDL_Texture *grass;
        SDL_Texture *water;

        int map[20][25];
    };
}


#endif //TOYPROJECTS_ROGUELIKE_MAP_H