//
// Created by 15813 on 2026/7/14.
//

#ifndef TOYPROJECTS_ROGUELIKE_COLLIDERCOMPONENT_H
#define TOYPROJECTS_ROGUELIKE_COLLIDERCOMPONENT_H

#include <string>
#include "SDL2/SDL.h"
#include "ECS.h"
#include "Components.h"

namespace roguelike::ECS {
    class ColliderComponent : public Component{
    public:
        SDL_Rect collider;
        std::string tag;
        TransformComponent* transform;

        ColliderComponent(std::string&& tag) : tag(tag){}

        void init() override{
            if (!entity->hasComponent<TransformComponent>()){
                entity->addComponent<TransformComponent>();
            }
            transform = &entity->getComponent<TransformComponent>();

            Game::colliders.push_back(this);
        }

        void update() override{
            collider.x = static_cast<int>(transform->getPos().x);
            collider.y = static_cast<int>(transform->getPos().y);
            collider.w = transform->width * transform->scale;
            collider.h = transform->height * transform->scale;
        }

    };
}
#endif //TOYPROJECTS_ROGUELIKE_COLLIDERCOMPONENT_H
