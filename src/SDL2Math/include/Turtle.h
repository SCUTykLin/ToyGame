//
// Created by 15813 on 2026/9/3.
//

#ifndef TOYPROJECTS_SDL_RAYTRACING_TURTLE_H
#define TOYPROJECTS_SDL_RAYTRACING_TURTLE_H
#include <SDL_render.h>

namespace Math::Display{
    class Turtle {
    public:
        Turtle();
        void SetRenderer(SDL_Renderer* m_pRenderer);
        void PenUp();
        void PenDown();
        void SetPenColor(int r, int g, int b, int alpha);
        void SetAngle(float angle);
        void SetLength(float length);
        void RotateRight(float angle);
        void RotateLeft(float angle);
        void MoveTo(float xPos, float yPos);
        void Step();
    private:
        SDL_Renderer* m_pRenderer;
        float m_x, m_y;
        float m_angle, m_length;
        int m_r, m_g, m_b, m_alpha;
        bool m_penDown;
    };
};


#endif //TOYPROJECTS_SDL_RAYTRACING_TURTLE_H
