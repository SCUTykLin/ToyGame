//
// Created by YangKai.Lin on 2026/7/19.
//

#ifndef TOYPROJECTS_Madsycode_ANIMATION_H
#define TOYPROJECTS_Madsycode_ANIMATION_H
#include <SDL_render.h>
#include <string>

namespace Madsycode::Animation {
    class Animation {
    public:
        Animation(){};
        void Update();
        void Draw(float x, float y, int spriteWidth, int spriteHeight);
        void SetProps(const std::string& textureID, int spriteRow, int frameCount, int animSpeed, SDL_RendererFlip flip=SDL_FLIP_NONE);
    private:
        int m_SpriteRow, m_SpriteFrame;
        int m_AnimSpeed, m_FrameCount;
        std::string m_TextureID;
        SDL_RendererFlip m_Flip;
    };
}


#endif //TOYPROJECTS_Madsycode_ANIMATION_H