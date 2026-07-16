//
// Created by YangKai.Lin on 2026/7/16.
//

#ifndef TOYPROJECTS_RPG_Platformer_MATH_H
#define TOYPROJECTS_RPG_Platformer_MATH_H
#include <iostream>

struct Vector2f
{
    Vector2f()
    :x(0.0f), y(0.0f)
    {}

    Vector2f(float p_x, float p_y)
    :x(p_x), y(p_y)
    {}

    void print()
    {
        std::cout << x << ", " << y << std::endl;
    }

    float x, y;
};
#endif //TOYPROJECTS_RPG_Platformer_MATH_H