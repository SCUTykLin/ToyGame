//
// Created by YangKai.Lin on 2026/7/19.
//

#include "Animation.h"

#include <SDL_timer.h>
#include "../Graphics/TextureManager.h"

namespace Madsycode::Animation {
    void Animation::Update() {
        m_SpriteFrame = (SDL_GetTicks() / m_AnimSpeed) % m_FrameCount;
    }

    void Animation::Draw(float x, float y, int spriteWidth, int spriteHeight) {
        Texture::TextureManager::GetInstance()->DrawFrame(m_TextureID, x, y, spriteWidth, spriteHeight, m_SpriteRow, m_SpriteFrame, m_Flip);
    }

    void Animation::SetProps(const std::string &textureID, int spriteRow, int frameCount,
        int animSpeed, SDL_RendererFlip flip) {
        m_TextureID = textureID;
        m_SpriteRow = spriteRow;
        m_FrameCount = frameCount;
        m_AnimSpeed = animSpeed;
        m_Flip = flip;
    }
}
