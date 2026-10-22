#pragma once
#include "gfx/Surface.h"

namespace gfx {

struct BitmapFont {
    const Surface* sheet = nullptr;
    int cw = 8;         // 单字宽
    int ch = 8;         // 单字高
    int cols = 15;      // 图集列数（固定 15）
};

// 返回绘制结束时的 x（方便接着画）
int draw_text(Surface& fb, const BitmapFont& f, int x, int y,
              const char* s, Pixel color);

// 九宫格面板：四角原样，四边拉伸，中心填充
void draw_panel(Surface& fb, Rect r, const Surface& sheet, int border);

// 三段式血条：左端盖 + 中间拉伸 + 右端盖
void draw_bar(Surface& fb, int x, int y, int w, int h, float ratio,
              const Surface& start, const Surface& mid, const Surface& end);

// 心形血条
void draw_hearts(Surface& fb, int x, int y, int hp, int max_hp,
                 const Surface& full, const Surface& empty);

} // namespace gfx
