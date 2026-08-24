//
// Created by YangKai.Lin on 2026/7/19.
//

#include "Animation.h"

#include <SDL_timer.h>
#include "../Graphics/TextureManager.h"

namespace Madsycode::Animation {
    void Animation::Update() {
        m_CurrentFrame = (SDL_GetTicks() / m_Speed) % m_FrameCount;
    }

    void Animation::Draw(float x, float y, int spriteWidth, int spriteHeight, SDL_RendererFlip flip) {
        Texture::TextureManager::GetInstance()->DrawFrame(m_TextureID, x, y, spriteWidth, spriteHeight, m_SpriteRow, m_CurrentFrame, flip);
    }

    void Animation::SetProps(const std::string &textureID, int spriteRow, int frameCount,
        int speed) {
        m_Speed = speed;
        m_TextureID = textureID;
        m_SpriteRow = spriteRow;
        m_FrameCount = frameCount;
        m_CurrentFrame = 0;
    }
}
