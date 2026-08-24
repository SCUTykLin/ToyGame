//
// Created by 15813 on 2026/7/14.
//

#ifndef TOYPROJECTS_ROGUELIKE_POSITIONCOMPONENTS_H
#define TOYPROJECTS_ROGUELIKE_POSITIONCOMPONENTS_H

#include "ECS.h"
#include "../App/Vector2D.h"

namespace roguelike::ECS {
    class TransformComponent : public Component {
    private:
        roguelike::calculate::Vector2D position;
        roguelike::calculate::Vector2D velocity;

    public:
        int height = 32;
        int width = 32;
        int scale = 1;
        int speed = 3;

        bool blocked = false;

        TransformComponent(){
            position.Zero();
        }

        TransformComponent(int sc){
            position.Zero();
            scale = sc;
        }

        TransformComponent(int x, int y){
            position.x = x;
            position.y = y;
        };

        roguelike::calculate::Vector2D& getPos(){
            return position;
        }

        roguelike::calculate::Vector2D& getVel(){
            return velocity;
        }

        TransformComponent(int x, int y, int h, int w, int sc){
            position.x = x;
            position.y = y;
            height = h;
            width = w;
            scale = sc;
        };


        void init() override{
            velocity.Zero();
        };

        void update() override {
            position += (velocity * speed);
        }

    };
}

#endif //TOYPROJECTS_ROGUELIKE_POSITIONCOMPONENTS_H
