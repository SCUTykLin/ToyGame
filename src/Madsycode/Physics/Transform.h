//
// Created by YangKai.Lin on 2026/7/18.
//

#ifndef TOYPROJECTS_Madsycode_TRANSFORM_H
#define TOYPROJECTS_Madsycode_TRANSFORM_H
#include "Vector2D.h"

namespace Madsycode::Physics {
    class Transform {
    public:
        float X, Y;
        Transform(float x, float y) : X(x), Y(y) {}
        inline void TranslateX(float x) { X += x; }
        inline void TranslateY(float y) { Y += y; }
        inline void Translate(Vector2D<float> v) { X += v.X; Y += v.Y; }
    };
}

#endif //TOYPROJECTS_Madsycode_TRANSFORM_H