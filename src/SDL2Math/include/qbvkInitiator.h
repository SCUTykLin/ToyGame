//
// Created by YangKai.Lin on 2026/10/31.
//

#ifndef TOYPROJECTS_SDL_RAYTRACING_QBVKINITIATOR_H
#define TOYPROJECTS_SDL_RAYTRACING_QBVKINITIATOR_H

#include <vector>
namespace Math::Display {
    class qbvkInitiator {
    public:
        qbvkInitiator();
        ~qbvkInitiator();

        void AddVertex(float xPos, float yPos);
        int NumEdge() const;
        bool GetEdge(float* x1, float* y1, float* x2, float* y2, int index);

    private:
        std::vector<float> m_vertexX;
        std::vector<float> m_vertexY;
    };
}

#endif //TOYPROJECTS_SDL_RAYTRACING_QBVKINITIATOR_H
