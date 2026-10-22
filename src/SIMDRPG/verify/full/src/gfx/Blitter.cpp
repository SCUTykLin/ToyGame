#include "gfx/Blitter.h"
#include "simd/Backend.h"

#include <cstring>

namespace gfx {

void blend_pixel_scalar(Pixel& d, Pixel s) {
    const uint32_t a = s >> 24;
    if (a == 0)   return;              // 全透明：跳过
    if (a == 255) { d = s; return; }   // 全不透明：直接覆盖

    const uint32_t inv = 255 - a;
    const uint32_t sr = (s >> 16) & 0xFF, sg = (s >> 8) & 0xFF, sb = s & 0xFF;
    const uint32_t dr = (d >> 16) & 0xFF, dg = (d >> 8) & 0xFF, db = d & 0xFF;

    d = 0xFF000000u
      | (((sr * a + dr * inv) / 255) << 16)
      | (((sg * a + dg * inv) / 255) << 8)
      |  ((sb * a + db * inv) / 255);
}

// 裁剪。返回 false 表示完全在屏幕外。
//
// 顺序不能反：先裁源矩形，再裁目标偏移。
// 反了会得到负的宽高，后面的循环一次都不执行，症状是"精灵消失了"。
static bool clip(Surface& dst, const Surface& src, Rect& r, int& dx, int& dy,
                 int& sx, int& sy, int& w, int& h) {
    // 1) 源矩形不能超出源图
    if (r.x < 0) { dx -= r.x; r.w += r.x; r.x = 0; }
    if (r.y < 0) { dy -= r.y; r.h += r.y; r.y = 0; }
    if (r.x + r.w > src.width())  r.w = src.width()  - r.x;
    if (r.y + r.h > src.height()) r.h = src.height() - r.y;

    sx = r.x; sy = r.y; w = r.w; h = r.h;

    // 2) 目标位置不能超出屏幕
    if (dx < 0) { sx -= dx; w += dx; dx = 0; }
    if (dy < 0) { sy -= dy; h += dy; dy = 0; }
    if (dx + w > dst.width())  w = dst.width()  - dx;
    if (dy + h > dst.height()) h = dst.height() - dy;

    return w > 0 && h > 0;
}

void blit_alpha(Surface& dst, const Surface& src, Rect r, int dx, int dy) {
    int sx, sy, w, h;
    if (!clip(dst, src, r, dx, dy, sx, sy, w, h)) return;

    for (int y = 0; y < h; ++y) {
        const Pixel* s = src.row(sy + y) + sx;
        Pixel*       d = dst.row(dy + y) + dx;
        simd::blend_row(d, s, w);          // ← 阶段 4 换的就是这一行
    }
}

void blit_opaque(Surface& dst, const Surface& src, Rect r, int dx, int dy) {
    int sx, sy, w, h;
    if (!clip(dst, src, r, dx, dy, sx, sy, w, h)) return;

    for (int y = 0; y < h; ++y) {
        std::memcpy(dst.row(dy + y) + dx, src.row(sy + y) + sx,
                    size_t(w) * sizeof(Pixel));
    }
}

void blit_tinted(Surface& dst, const Surface& src, Rect r, int dx, int dy, Pixel tint) {
    int sx, sy, w, h;
    if (!clip(dst, src, r, dx, dy, sx, sy, w, h)) return;

    const uint32_t tr = (tint >> 16) & 0xFF;
    const uint32_t tg = (tint >> 8) & 0xFF;
    const uint32_t tb = tint & 0xFF;

    for (int y = 0; y < h; ++y) {
        const Pixel* s = src.row(sy + y) + sx;
        Pixel*       d = dst.row(dy + y) + dx;
        for (int x = 0; x < w; ++x) {
            const Pixel p = s[x];
            const uint32_t a = p >> 24;
            if (a == 0) continue;
            const uint32_t rr = (((p >> 16) & 0xFF) * tr) / 255;
            const uint32_t gg = (((p >> 8) & 0xFF) * tg) / 255;
            const uint32_t bb = ((p & 0xFF) * tb) / 255;
            blend_pixel_scalar(d[x], (a << 24) | (rr << 16) | (gg << 8) | bb);
        }
    }
}

void blit_mask(Surface& dst, const Surface& src, Rect r, int dx, int dy, Pixel color) {
    int sx, sy, w, h;
    if (!clip(dst, src, r, dx, dy, sx, sy, w, h)) return;

    const Pixel rgb = color & 0x00FFFFFFu;
    for (int y = 0; y < h; ++y) {
        const Pixel* srow = src.row(sy + y);
        Pixel*       drow = dst.row(dy + y);
        for (int x = 0; x < w; ++x) {
            const uint8_t a = core::alpha_of(srow[sx + x]);
            if (!a) continue;                       // 全透明的像素跳过
            blend_pixel_scalar(drow[dx + x], (Pixel(a) << 24) | rgb);
        }
    }
}

void fill_rect(Surface& dst, Rect r, Pixel color) {
    const int x0 = r.x < 0 ? 0 : r.x;
    const int y0 = r.y < 0 ? 0 : r.y;
    const int x1 = r.x + r.w > dst.width()  ? dst.width()  : r.x + r.w;
    const int y1 = r.y + r.h > dst.height() ? dst.height() : r.y + r.h;

    for (int y = y0; y < y1; ++y) {
        Pixel* d = dst.row(y);
        for (int x = x0; x < x1; ++x) d[x] = color;
    }
}

void fill_rect_alpha(Surface& dst, Rect r, Pixel color) {
    const int a = int(core::alpha_of(color));
    if (a == 0) return;                        // 全透明：什么也不做
    if (a == 255) { fill_rect(dst, r, color); return; }

    const int x0 = r.x < 0 ? 0 : r.x;
    const int y0 = r.y < 0 ? 0 : r.y;
    const int x1 = r.x + r.w > dst.width()  ? dst.width()  : r.x + r.w;
    const int y1 = r.y + r.h > dst.height() ? dst.height() : r.y + r.h;
    if (x0 >= x1 || y0 >= y1) return;

    for (int y = y0; y < y1; ++y) {
        Pixel* d = dst.row(y);
        for (int x = x0; x < x1; ++x) blend_pixel_scalar(d[x], color);
    }
}

void blit_stretch(Surface& dst, const Surface& src, Rect s, Rect d) {
    if (d.w <= 0 || d.h <= 0 || s.w <= 0 || s.h <= 0) return;

    for (int y = 0; y < d.h; ++y) {
        const int dy = d.y + y;
        if (dy < 0 || dy >= dst.height()) continue;      // 纵向裁剪，漏了会段错误

        const int sy = s.y + y * s.h / d.h;              // 最近邻采样
        if (sy < 0 || sy >= src.height()) continue;

        const Pixel* srow = src.row(sy);
        Pixel*       drow = dst.row(dy);
        for (int x = 0; x < d.w; ++x) {
            const int dx = d.x + x;
            if (dx < 0 || dx >= dst.width()) continue;   // 横向裁剪
            const int sx = s.x + x * s.w / d.w;
            if (sx < 0 || sx >= src.width()) continue;
            blend_pixel_scalar(drow[dx], srow[sx]);
        }
    }
}

} // namespace gfx
