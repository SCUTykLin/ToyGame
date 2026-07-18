//
// Created by YangKai.Lin on 2026/7/17.
//

#include "TextureManager.h"

#include <SDL_image.h>

#include "../Core/Engine.h"

namespace Madsycode::Texture {
    TextureManager* TextureManager::s_Instance = nullptr;
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

        m_Textures[id] = texture;
        return true;
    }

    void TextureManager::Drop(const std::string &id) {
        SDL_DestroyTexture(m_Textures[id]);
        m_Textures.erase(id);
    }

    void TextureManager::Clean() {
        for (auto it = m_Textures.begin(); it != m_Textures.end(); ++it){
            SDL_DestroyTexture(it->second);
        }
        m_Textures.clear();
        SDL_Log("Texture map cleaned!");
    }

    void TextureManager::Draw(const std::string &id, int x, int y, int width, int height, SDL_RendererFlip flip) {
        SDL_Rect srcRect{0, 0, width, height};
        SDL_Rect dstRect{x, y, width, height};
        SDL_RenderCopyEx(Engine::Engine::GetInstance()->GetRenderer(), m_Textures[id], &srcRect, &dstRect, 0, nullptr, flip);

    }
}
