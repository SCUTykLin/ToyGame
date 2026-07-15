//
// Created by YangKai.Lin on 2026/7/15.
//

#ifndef TOYPROJECTS_ROGUELIKE_ANIMATION_H
#define TOYPROJECTS_ROGUELIKE_ANIMATION_H
struct Animation
{

    int index;
    int frames;
    int speed;

    Animation() {}
    Animation(int i, int f, int s)
    {
        index = i;
        frames = f;
        speed = s;
    }

};
#endif //TOYPROJECTS_ROGUELIKE_ANIMATION_H