//
// Created by 15813 on 2026/7/15.
//

#ifndef TOYPROJECTS_ROGUELIKE_PROJECTILECOMPONENT_H
#define TOYPROJECTS_ROGUELIKE_PROJECTILECOMPONENT_H

#include "ECS.h"
#include "Components.h"
#include "../App/Vector2D.h"

namespace roguelike::ECS{
    class ProjectileComponent : public Component{
    public:
        ProjectileComponent(int rng, int sp, calculate::Vector2D vel) : range(rng), speed(sp), velocity(vel){}
        ~ProjectileComponent(){}

        void init() override
        {
            transform = &entity->getComponent<TransformComponent>();
            transform->getVel() = velocity;
        }

        void update() override
        {
            distance += speed;

            if (distance > range)
            {
                std::cout << "Out of Range" << std::endl;
                entity->destroy();
            }
            else if (transform->getPos().x > Game::camera.x + Game::camera.w ||
                     transform->getPos().x < Game::camera.x ||
                     transform->getPos().y > Game::camera.y + Game::camera.h ||
                     transform->getPos().y < Game::camera.y)
            {
                std::cout << "Out of bounds!" << std::endl;
                entity->destroy();
            }
        }
    private:

        TransformComponent* transform;

        int range = 0;
        int speed = 0;
        int distance = 0;
        calculate::Vector2D velocity;


    };
}

#endif //TOYPROJECTS_ROGUELIKE_PROJECTILECOMPONENT_H
