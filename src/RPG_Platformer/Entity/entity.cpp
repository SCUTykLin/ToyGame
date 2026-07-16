//
// Created by YangKai.Lin on 2026/7/16.
//

#include "entity.h"

namespace RPG_Platformer {
    Entity::Entity(Vector2f p_pos, SDL_Texture* p_tex)
    :pos(p_pos), tex(p_tex)
    {
        currentFrame.x = 0;
        currentFrame.y = 0;
        currentFrame.w = 32;
        currentFrame.h = 32;
    }

    void Entity::update() {
        currentFrame.w = 64;
        currentFrame.h = 64;
        if (status == "stay") {
            currentFrame.y = 0;
        }else if (status == "walk") {
            currentFrame.y = 1;
        }
        currentFrame.y *= 64;
        currentFrame.x = currentFrame.w * static_cast<int>((SDL_GetTicks() / 100) % num_frames);
    }

    SDL_Texture* Entity::getTex(){
        return tex;
    }

    SDL_Rect Entity::getCurrentFrame()
    {
        return currentFrame;
    }
}