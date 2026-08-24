//
// Created by 15813 on 2026/7/19.
//

#ifndef TOYPROJECTS_Madsycode_COLLIDER_H
#define TOYPROJECTS_Madsycode_COLLIDER_H

#include "SDL2/SDL.h"

namespace Madsycode::Physics {
    class Collider {
    public:
        inline SDL_Rect Get() {return m_Box;}
        inline void SetBuffer(int x, int y, int w, int h) {m_Buffer = {x, y, w, h};}
        void Set(int x, int y, int w, int h){
            m_Box = {x - m_Buffer.x,
                     y - m_Buffer.y,
                     w - m_Buffer.w,
                     h - m_Buffer.h};
        }
    private:
        SDL_Rect m_Box;
        SDL_Rect m_Buffer; // 有些texture的碰撞边框和他的图片差别很大，可以通过buffer调整
    };
}

#endif //TOYPROJECTS_Madsycode_COLLIDER_H
