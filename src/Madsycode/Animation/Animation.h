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
        void Draw(float x, float y, int spriteWidth, int spriteHeight, SDL_RendererFlip flip);
        void SetProps(const std::string& textureID, int spriteRow, int frameCount, int speed);
    private:
        int m_Speed;
        int m_SpriteRow;
        int m_FrameCount;
        int m_CurrentFrame;
        std::string m_TextureID;
    };
}


#endif //TOYPROJECTS_Madsycode_ANIMATION_H