//
// Created by YangKai.Lin on 2026/7/16.
//

#ifndef TOYPROJECTS_RPG_Platformer_ENTITY_H
#define TOYPROJECTS_RPG_Platformer_ENTITY_H

#include "SDL2/SDL.h"
#include "SDL_image.h"
#include "Math.h"

namespace RPG_Platformer {
    class Entity {
    public:
        Entity(Vector2f p_pos, SDL_Texture* p_tex);
        Vector2f& getPos(){return pos;}
        SDL_Texture* getTex();
        void update();
        SDL_Rect getCurrentFrame();
    private:
        Vector2f pos;
        SDL_Rect currentFrame;
        SDL_Texture* tex;
        std::string status = "stay";
        int num_frames = 5;
        bool flip = false;
    };
}

#endif //TOYPROJECTS_RPG_Platformer_ENTITY_H