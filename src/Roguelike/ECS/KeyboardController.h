//
// Created by 15813 on 2026/7/14.
//

#ifndef TOYPROJECTS_ROGUELIKE_KEYBOARDCONTROLLER_H
#define TOYPROJECTS_ROGUELIKE_KEYBOARDCONTROLLER_H

#include "../Game/game.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"

namespace roguelike::ECS {
    class KeyboardController : public Component {
    public:
        TransformComponent *transform;
        SpriteComponent* sprite;

        void init() override {
            transform = &entity->getComponent<TransformComponent>();
            sprite = &entity->getComponent<SpriteComponent>();
        }

        void update() override {
            if (Game::event.type == SDL_KEYDOWN) {
                switch (Game::event.key.keysym.sym) {
                    case SDLK_w:
                        transform->getVel().y = -1;
                        sprite->Play("Walk");
                        break;
                    case SDLK_a:
                        transform->getVel().x = -1;
                        sprite->Play("Walk");
                        sprite->spriteFlip = SDL_FLIP_HORIZONTAL;
                        break;
                    case SDLK_d:
                        transform->getVel().x = 1;
                        sprite->Play("Walk");
                        break;
                    case SDLK_s:
                        transform->getVel().y = 1;
                        sprite->Play("Walk");
                        break;
                    default:
                        break;
                }
            }

            if (Game::event.type == SDL_KEYUP) {
                switch (Game::event.key.keysym.sym) {
                    case SDLK_w:
                        transform->getVel().y = 0;
                        sprite->Play("Idle");
                        break;
                    case SDLK_a:
                        transform->getVel().x = 0;
                        sprite->Play("Idle");
                        sprite->spriteFlip = SDL_FLIP_NONE;
                        break;
                    case SDLK_d:
                        transform->getVel().x = 0;
                        sprite->Play("Idle");
                        break;
                    case SDLK_s:
                        transform->getVel().y = 0;
                        sprite->Play("Idle");
                        break;
                    case SDLK_ESCAPE:
                        Game::isRunning = false;
                    default:
                        break;
                }
            }
        }
    };
}
#endif //TOYPROJECTS_ROGUELIKE_KEYBOARDCONTROLLER_H