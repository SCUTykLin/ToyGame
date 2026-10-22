#include "gfx/Ui.h"
#include "gfx/Blitter.h"

namespace gfx {

int draw_text(Surface& fb, const BitmapFont& f, int x, int y,
              const char* s, Pixel color) {
    if (!f.sheet || !s) return x;

    const int start_x = x;

    for (; *s; ++s) {
        if (*s == '\n') {
            x = start_x;
            y += f.ch;
            continue;
        }

        const int idx = int(static_cast<unsigned char>(*s)) - 32;
        if (idx < 0 || idx >= 95) {     // 非 ASCII（含中文）：留空跳过
            x += f.cw;
            continue;
        }

        const Rect src{ (idx % f.cols) * f.cw,
                        (idx / f.cols) * f.ch,
                        f.cw, f.ch };

        // 用 blit_mask 而不是 blit_tinted：
        // 字形是"形状"，颜色由我们指定。乘法染色对深色字形无效。
        blit_mask(fb, *f.sheet, src, x, y, color);
        x += f.cw;
    }

    return x;
}

void draw_panel(Surface& fb, Rect r, const Surface& sheet, int border) {
    const int sw = sheet.width();
    const int sh = sheet.height();
    const int b  = border;

    if (b <= 0 || sw < 2 * b || sh < 2 * b || r.w < 2 * b || r.h < 2 * b) return;

    // 四角：原样贴，不拉伸（拉伸会让圆角变形）
    blit_opaque(fb, sheet, {0, 0, b, b},                 r.x,             r.y);
    blit_opaque(fb, sheet, {sw - b, 0, b, b},            r.x + r.w - b,   r.y);
    blit_opaque(fb, sheet, {0, sh - b, b, b},            r.x,             r.y + r.h - b);
    blit_opaque(fb, sheet, {sw - b, sh - b, b, b},       r.x + r.w - b,   r.y + r.h - b);

    // 四条边：沿一个方向拉伸
    blit_stretch(fb, sheet, {b, 0, sw - 2 * b, b},
                 {r.x + b, r.y, r.w - 2 * b, b});
    blit_stretch(fb, sheet, {b, sh - b, sw - 2 * b, b},
                 {r.x + b, r.y + r.h - b, r.w - 2 * b, b});
    blit_stretch(fb, sheet, {0, b, b, sh - 2 * b},
                 {r.x, r.y + b, b, r.h - 2 * b});
    blit_stretch(fb, sheet, {sw - b, b, b, sh - 2 * b},
                 {r.x + r.w - b, r.y + b, b, r.h - 2 * b});

    // 中心
    blit_stretch(fb, sheet, {b, b, sw - 2 * b, sh - 2 * b},
                 {r.x + b, r.y + b, r.w - 2 * b, r.h - 2 * b});
}

void draw_bar(Surface& fb, int x, int y, int w, int h, float ratio,
              const Surface& start, const Surface& mid, const Surface& end) {
    if (ratio < 0.f) ratio = 0.f;
    if (ratio > 1.f) ratio = 1.f;

    const int cap_l = start.width();
    const int cap_r = end.width();
    const int fill  = int(float(w - cap_l - cap_r) * ratio);

    blit_opaque(fb, start, {0, 0, cap_l, start.height()}, x, y);
    if (fill > 0) {
        blit_stretch(fb, mid, {0, 0, mid.width(), mid.height()},
                     {x + cap_l, y, fill, h});
    }
    blit_opaque(fb, end, {0, 0, cap_r, end.height()}, x + cap_l + fill, y);
}

void draw_hearts(Surface& fb, int x, int y, int hp, int max_hp,
                 const Surface& full, const Surface& empty) {
    if (max_hp <= 0) return;

    constexpr int PER_HEART = 20;
    const int n = (max_hp + PER_HEART - 1) / PER_HEART;
    const int step = full.width() + 2;

    for (int i = 0; i < n; ++i) {
        const int left = hp - i * PER_HEART;
        const Surface& img = (left >= PER_HEART / 2) ? full : empty;
        blit_alpha(fb, img, {0, 0, img.width(), img.height()},
                   x + i * step, y);
    }
}

} // namespace gfx
