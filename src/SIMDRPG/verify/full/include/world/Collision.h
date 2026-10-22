#pragma once
#include "core/Types.h"

namespace world {

class TileMap;

// 精灵左上角在 (px, py) 时，脚底碰撞盒的位置
inline core::Rect feet(float px, float py) {
    return { int(px) + 16, int(py) + 40, 32, 20 };
}

// 矩形是否与地图的实心格重叠
bool box_solid(core::Rect r, const TileMap& m);

// 分轴移动。返回是否真的移动了。
// flying = true 时忽略地形（飞行怪）
bool try_move(float& x, float& y, float vx, float vy,
              const TileMap& m, bool flying = false);

} // namespace world
