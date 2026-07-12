//
// Created by YangKai.Lin on 2026/7/12.
//

#include "gameObject.h"
#include "../TextureManager/textureManager.h"

namespace roguelike {
    GameObject::GameObject(const char *texturesheet, SDL_Renderer *renderer) {
        objTexture = TextureManager::LoadTexture(texturesheet, renderer);
    }

    GameObject::~GameObject() {
    }
}
