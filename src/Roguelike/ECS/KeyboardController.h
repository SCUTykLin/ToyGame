//
// Created by 15813 on 2026/7/14.
//

#ifndef TOYPROJECTS_ROGUELIKE_KEYBOARDCONTROLLER_H
#define TOYPROJECTS_ROGUELIKE_KEYBOARDCONTROLLER_H

#include "TransformComponent.h"
#include "../Game/game.h"

namespace roguelike::ECS {
    class KeyboardController : public Component {
    public:
        TransformComponent *transform;

        void init() override {
            transform = &entity->getComponent<TransformComponent>();
        }

        void update() override {
            if (Game::event.type == SDL_KEYDOWN) {
                switch (Game::event.key.keysym.sym) {
                    case SDLK_w:
                        transform->getVel().y = -1;
                        break;
                    case SDLK_a:
                        transform->getVel().x = -1;
                        break;
                    case SDLK_d:
                        transform->getVel().x = 1;
                        break;
                    case SDLK_s:
                        transform->getVel().y = 1;
                        break;
                    default:
                        break;
                }
            }
            if (Game::event.type == SDL_KEYUP) {
                switch (Game::event.key.keysym.sym) {
                    case SDLK_w:
                        transform->getVel().y = 0;
                        break;
                    case SDLK_a:
                        transform->getVel().x = 0;
                        break;
                    case SDLK_d:
                        transform->getVel().x = 0;
                        break;
                    case SDLK_s:
                        transform->getVel().y = 0;
                        break;
                    default:
                        break;
                }
            }
        }
    };
}
#endif //TOYPROJECTS_ROGUELIKE_KEYBOARDCONTROLLER_H