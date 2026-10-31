//
// Created by YangKai.Lin on 2026/10/31.
//

#ifndef TOYPROJECTS_SDL_RAYTRACING_DRAGONCURVE_H
#define TOYPROJECTS_SDL_RAYTRACING_DRAGONCURVE_H

#include <vector>
#include "Turtle.h"

namespace Math::Display {
    class DragonCurve {
    public:
        DragonCurve();
        ~DragonCurve();

        void SetStartPoint(float xPos, float yPos);
        void SetLength(float length);
        void SetLevel(int level);
        bool Compute();
        int NumEdges();
        bool GetEdge(float* x1, float* y1, float* x2, float* y2, int index);
    private:
        void BuildTurns();

        std::vector<int> m_turnList;
        std::vector<float> m_x1;
        std::vector<float> m_y1;
        std::vector<float> m_x2;
        std::vector<float> m_y2;
        Turtle m_turtle;
        float m_startX, m_startY;
        float m_length;
        int m_level;
    };
}

#endif //TOYPROJECTS_SDL_RAYTRACING_DRAGONCURVE_H
