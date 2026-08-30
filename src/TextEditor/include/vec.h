//
// Created by YangKai.Lin on 2026/8/30.
//

#ifndef TOYPROJECTS_TEXEDITOR_VEC_H
#define TOYPROJECTS_TEXEDITOR_VEC_H

namespace TextEditor {
    typedef struct {
        float x;
        float y;
    }Vec2f;

    Vec2f vec2f(float x, float y);
    Vec2f vec2fs(float x);
    Vec2f vec2f_add(Vec2f a, Vec2f b);
    Vec2f vec2f_sub(Vec2f a, Vec2f b);
    Vec2f vec2f_mul(Vec2f a, Vec2f b);
    Vec2f vec2f_div(Vec2f a, Vec2f b);

}

#endif //TOYPROJECTS_TEXEDITOR_VEC_H
