#pragma once
#include <cstdint>

namespace core {

struct Vec2i { int x, y; };
struct Vec2f { float x, y; };

// 全项目唯一的矩形定义。
// 不要在任何别的头文件里再写一遍 struct Rect —— 两个不同的 Rect
// 会导致"明明参数对得上却编译不过"，而且报错信息极难看懂。
struct Rect { int x, y, w, h; };

// 像素格式：0xAARRGGBB。小端内存里字节顺序是 B,G,R,A
using Pixel = uint32_t;

constexpr Pixel rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
    return (Pixel(a) << 24) | (Pixel(r) << 16) | (Pixel(g) << 8) | Pixel(b);
}

constexpr uint8_t alpha_of(Pixel p) { return uint8_t(p >> 24); }
constexpr uint8_t red_of  (Pixel p) { return uint8_t((p >> 16) & 0xFF); }
constexpr uint8_t green_of(Pixel p) { return uint8_t((p >>  8) & 0xFF); }
constexpr uint8_t blue_of (Pixel p) { return uint8_t( p        & 0xFF); }

// 两个矩形是否相交（战斗判定到处在用）
inline bool rect_overlap(Rect a, Rect b) {
    return a.x < b.x + b.w && b.x < a.x + a.w &&
           a.y < b.y + b.h && b.y < a.y + a.h;
}

// 点是否在矩形内
inline bool rect_contains(Rect r, int px, int py) {
    return px >= r.x && py >= r.y && px < r.x + r.w && py < r.y + r.h;
}

} // namespace core
