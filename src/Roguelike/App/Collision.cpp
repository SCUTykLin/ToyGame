//
// Created by 15813 on 2026/7/14.
//

#include "Collision.h"
#include "../ECS/ColliderComponent.h"

namespace roguelike::collision{
    bool Collision::AABB(const SDL_Rect &recA, const SDL_Rect &recB) {
        if (recA.x + recA.w >= recB.x &&
            recB.x + recB.w >= recA.x &&
            recA.y + recA.h >= recB.y &&
            recB.y + recB.h >= recA.y){
            return true;
        }
        return false;
    }

    bool Collision::AABB(const ECS::ColliderComponent &colA, const ECS::ColliderComponent &colB) {
        return AABB(colA.collider, colB.collider);
    }
}