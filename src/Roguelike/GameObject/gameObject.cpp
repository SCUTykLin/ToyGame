//
// Created by YangKai.Lin on 2026/7/12.
//

#include "gameObject.h"
#include "../TextureManager/textureManager.h"

namespace roguelike {
    GameObject::GameObject(const char *texturesheet, int x, int y) {
        objTexture = TextureManager::LoadTexture(texturesheet);
        xpos = x;
        ypos = y;
    }

    GameObject::~GameObject() {
    }

    void GameObject::Update() {
        xpos ++;
        ypos ++;

        srcRect.h = 32;
        srcRect.w = 32;
        srcRect.x = 0;
        srcRect.y = 0;

        destRect.x = xpos;
        destRect.y = ypos;
        destRect.w = srcRect.w * 2;
        destRect.h = srcRect.h * 2;
    }
}
