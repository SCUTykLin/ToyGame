//
// Created by YangKai.Lin on 2026/7/17.
//

#include "TextureManager.h"

#include <SDL_image.h>

#include "../Core/Engine.h"

namespace Madsycode::Texture {
    bool TextureManager::Load(const std::string &id, const std::string &filename) {
        SDL_Surface* surface = IMG_Load(filename.c_str());
        if (surface == nullptr) {
            SDL_Log("Failed to load image: %s", SDL_GetError());
            return false;
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(Madsycode::Engine::Engine::GetInstance()->GetRenderer(), surface);
        if (texture == nullptr) {
            SDL_Log("Failed to create texture from surface: %s", SDL_GetError());
            return false;
        }
    }

    void TextureManager::Drop(const std::string &id) {
    }

    void TextureManager::Clean() {
    }

    void TextureManager::Draw(const std::string &id, int x, int y, int width, int height, SDL_RendererFlip flip) {
    }
}
