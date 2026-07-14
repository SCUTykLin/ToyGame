//
// Created by 15813 on 2026/7/14.
//

#ifndef TOYPROJECTS_ROGUELIKE_COLLISION_H
#define TOYPROJECTS_ROGUELIKE_COLLISION_H

#include "SDL2/SDL.h"

namespace roguelike::ECS{
    class ColliderComponent;
}

namespace roguelike::collision{

    class Collision{
    public:
        static bool AABB(const SDL_Rect& recA, const SDL_Rect& recB);
        static bool AABB(const roguelike::ECS::ColliderComponent& colA,
                         const roguelike::ECS::ColliderComponent& colB);
    };
}

#endif //TOYPROJECTS_ROGUELIKE_COLLISION_H
