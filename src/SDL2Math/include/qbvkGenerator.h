//
// Created by YangKai.Lin on 2026/10/31.
//

#ifndef TOYPROJECTS_SDL_RAYTRACING_QBVKGENERATOR_H
#define TOYPROJECTS_SDL_RAYTRACING_QBVKGENERATOR_H

#include <vector>
#include "Turtle.h"

namespace Math::Display {
    class qbvkGenerator {
    public:
        qbvkGenerator();
        ~qbvkGenerator();

        void SetStartPoint(float xPos, float yPos);
        void SetEndPoint(float xPos, float yPos);
        void AddNode(float angle);
        bool Compute();
        int NumEdges();
        bool GetEdge(float* x1, float* y1, float* x2, float* y2, int index);
    private:
        std::vector<float> m_nodeAngle;
        std::vector<float> m_x1;
        std::vector<float> m_y1;
        std::vector<float> m_x2;
        std::vector<float> m_y2;
        Turtle m_turtle;
        float m_startX, m_startY;
        float m_endX, m_endY;
        int m_numEdges;
    };
}

#endif //TOYPROJECTS_SDL_RAYTRACING_QBVKGENERATOR_H
