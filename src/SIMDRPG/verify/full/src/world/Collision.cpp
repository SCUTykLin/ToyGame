#include "world/Collision.h"
#include "world/TileMap.h"

namespace world {

bool box_solid(core::Rect r, const TileMap& m) {
    const int ts = TileMap::TS;

    const int x0 = r.x / ts;
    const int y0 = r.y / ts;

    // 注意 -1：右边界是"最后一个像素的坐标"，不是"右边界外的第一个像素"。
    // 一个 32 宽的盒子放在 x=0，覆盖像素 0..31，只占第 0 格。
    // 写成 (r.x + r.w) / ts 的话会算成第 1 格，角色会"隔一格撞墙"。
    const int x1 = (r.x + r.w - 1) / ts;
    const int y1 = (r.y + r.h - 1) / ts;

    for (int ty = y0; ty <= y1; ++ty)
        for (int tx = x0; tx <= x1; ++tx)
            if (m.is_solid(tx, ty)) return true;

    return false;
}

bool try_move(float& x, float& y, float vx, float vy,
              const TileMap& m, bool flying) {
    bool moved = false;

    if (vx != 0.f) {
        const float nx = x + vx;
        if (flying || !box_solid(feet(nx, y), m)) {
            x = nx;
            moved = true;
        }
    }

    if (vy != 0.f) {
        const float ny = y + vy;
        if (flying || !box_solid(feet(x, ny), m)) {
            y = ny;
            moved = true;
        }
    }

    return moved;
}

} // namespace world
