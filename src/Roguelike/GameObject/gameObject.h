//
// Created by YangKai.Lin on 2026/7/12.
//

#ifndef TOYPROJECTS_ROGUELIKE_GAMEOBJECT_H
#define TOYPROJECTS_ROGUELIKE_GAMEOBJECT_H

#include "../Game/game.h"

class GameObject {
public:
    GameObject(const char* texturesheet, SDL_Renderer* renderer);
    ~GameObject();
private:
    int xpos;
    int ypos;
    SDL_Texture* objTexture;
    SDL_Rect srcRect, destRect;
};

#endif //TOYPROJECTS_ROGUELIKE_GAMEOBJECT_H