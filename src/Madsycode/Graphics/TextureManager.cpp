//
// Created by YangKai.Lin on 2026/7/17.
//

#include "TextureManager.h"

#include <SDL_image.h>

#include "../Core/Engine.h"
#include "../Camera/Camera.h"

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
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        m_Textures[id] = texture;
        return true;
    }

    bool TextureManager::ParseTexture(const std::string &&source) {
        return false;
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

    /*
     * 世界坐标系应该是以画布，也就是背景的左上角为原点。
     * */
    void TextureManager::Draw(const std::string &id, int x, int y, int width, int height, SDL_RendererFlip flip) {
        SDL_Rect srcRect{0, 0, width, height};
        Physics::Vector2D<float> cam = Camera::Camera::GetInstance()->GetPosition();
        SDL_Rect dstRect = {static_cast<int>(x - cam.X), static_cast<int>(y - cam.Y), width, height}; // world to screen coordinate
        SDL_RenderCopyEx(Engine::Engine::GetInstance()->GetRenderer(), m_Textures[id],
            &srcRect, &dstRect, 0,nullptr, flip);
    }

    void TextureManager::DrawTile(const std::string &tilesetID, int tileSize,
        int x, int y,
        int row, int frame,
        SDL_RendererFlip flip) {

        Physics::Vector2D<float> cam = Camera::Camera::GetInstance()->GetPosition();

        SDL_Rect desRect = {static_cast<int>(x - cam.X), static_cast<int>(y - cam.Y),
                            static_cast<int>(tileSize), static_cast<int>(tileSize)};
        SDL_Rect srcRect {17 * frame, 17 * row, 17, 17};

        SDL_RenderCopyEx(Engine::Engine::GetInstance()->GetRenderer(), m_Textures[tilesetID],
            &srcRect, &desRect, 0, nullptr, flip);
    }

    void TextureManager::DrawFrame(const std::string &id, int x, int y, int width, int height, int row, int frame,
                                   SDL_RendererFlip flip) {
        SDL_Rect srcRect{width * frame, height * (row-1), width, height};
        Physics::Vector2D<float> cam = Camera::Camera::GetInstance()->GetPosition();
        SDL_Rect dstRect{static_cast<int>(x - cam.X), static_cast<int>(y - cam.Y), width, height};
        SDL_RenderCopyEx(Engine::Engine::GetInstance()->GetRenderer(), m_Textures[id],
            &srcRect, &dstRect, 0,nullptr, flip);
    }

}
