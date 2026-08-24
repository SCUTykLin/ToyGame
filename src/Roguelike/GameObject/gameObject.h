//
// Created by YangKai.Lin on 2026/7/12.
//

#ifndef TOYPROJECTS_ROGUELIKE_GAMEOBJECT_H
#define TOYPROJECTS_ROGUELIKE_GAMEOBJECT_H

#include "../Game/game.h"

namespace roguelike {
    class GameObject {
    public:
        GameObject(const char* texturesheet, int x, int y);
        ~GameObject();
        void Update();

        SDL_Texture* get_objTexture(){return objTexture;}

        SDL_Rect& get_srcRect(){return srcRect;}

        SDL_Rect& get_destRect(){return destRect;}
    private:
        int xpos;
        int ypos;
        SDL_Texture* objTexture;
        SDL_Rect srcRect, destRect;
    };
}

#endif //TOYPROJECTS_ROGUELIKE_GAMEOBJECT_H