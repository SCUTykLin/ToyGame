//
// Created by 15813 on 2026/7/19.
//

#ifndef TOYPROJECTS_Madsycode_COLLISIONHANDLER_H
#define TOYPROJECTS_Madsycode_COLLISIONHANDLER_H

#include "SDL2/SDL.h"
#include "../Map/TileLayer.h"
#include "../Map/GameMap.h"

/*
 * 这里的碰撞检测做了简化，只进行了静态检测，并没有进行动态碰撞，可能会出现隧穿问题。
 * */

namespace Madsycode::Collision {
    class CollisionHandler {
    public:
        bool CheckCollision(SDL_Rect a, SDL_Rect b);
        bool MapCollision (SDL_Rect a);
        inline static CollisionHandler* GetInstance(){
            return s_Instance = (s_Instance != nullptr) ? s_Instance : new CollisionHandler();
        }
    private:
        CollisionHandler();
        Map::TileMap m_CollisionTilemap;
        Map::TileLayer* m_CollisionLayer;
        static CollisionHandler* s_Instance;
    };
}


#endif //TOYPROJECTS_Madsycode_COLLISIONHANDLER_H
