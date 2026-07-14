//
// Created by 15813 on 2026/7/14.
//

#ifndef TOYPROJECTS_ROGUELIKE_TILECOMPONENT_H
#define TOYPROJECTS_ROGUELIKE_TILECOMPONENT_H

#include "ECS.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"

namespace roguelike::ECS {
    class TileComponent : public Component{
    public:
        TransformComponent* transform;
        SpriteComponent* sprite;

        SDL_Rect tileRect;
        int tileID;
        char* path;

        TileComponent() = default;
        TileComponent(int x, int y, int w, int h, int id){
            tileRect.x = x;
            tileRect.y = y;
            tileRect.h = h;
            tileRect.w = w;
            tileID = id;

            switch(tileID){
                case 0:
                    path = "src/Roguelike/Assets/water.png";
                    break;
                case 1:
                    path = "src/Roguelike/Assets/water/dirt.png";
                    break;
                case 2:
                    path = "src/Roguelike/Assets/water/grass.png";
                    break;
                default:
                    break;
            }
        }

        void init() override{
            entity->addComponent<TransformComponent>(tileRect.x, tileRect.y, tileRect.w, tileRect.h, 1);
            transform = &entity->getComponent<TransformComponent>();

        }
    };
}
#endif //TOYPROJECTS_ROGUELIKE_TILECOMPONENT_H
