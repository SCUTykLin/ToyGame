#pragma once
#include "gfx/Surface.h"
#include "core/Types.h"

namespace game {

// 只管"现在该取哪一帧"，不管绘制。
struct Animator {
    const gfx::Surface* sheet = nullptr;

    int fw = 64, fh = 64;      // 单帧尺寸
    int frames = 8;            // 每行几帧
    int rows = 4;              // 图集有几行（LPC 角色 4 行，怪物 1 行）

    float fps = 8.f;
    int col = 0;
    int row = 2;               // 默认朝下（正面）
    float timer = 0.f;

    void setup(const gfx::Surface* s, int w, int h, int n, int r) {
        sheet = s; fw = w; fh = h; frames = n; rows = r;
        col = 0; row = (r >= 4) ? 2 : 0;
        timer = 0.f;
    }

    void update(float dt, bool moving, int dx, int dy) {
        // 只有 4 行图集才有方向；怪物图集 1 行，永远 row=0
        if (rows >= 4 && moving) {
            if      (dy < 0) row = 0;   // 上 → 背
            else if (dy > 0) row = 2;   // 下 → 正
            else if (dx < 0) row = 1;   // 左
            else if (dx > 0) row = 3;   // 右
        }

        // 4 行角色：站着不动就停在第一帧
        // 1 行怪物：一直动，否则看起来像贴图
        const bool animate = moving || rows < 4;

        if (!animate) {
            col = 0;
            timer = 0.f;
            return;
        }

        timer += dt;
        const float step = 1.f / fps;
        while (timer >= step) {
            timer -= step;
            col = (col + 1) % frames;
        }
    }

    core::Rect rect() const {
        return { col * fw, row * fh, fw, fh };
    }
};

} // namespace game
