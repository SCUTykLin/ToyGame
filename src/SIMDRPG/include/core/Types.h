//
// Created by 15813 on 2026-10-28.
//

#ifndef TOYPROJECTS_SIMDRPG_TYPES_H
#define TOYPROJECTS_SIMDRPG_TYPES_H
#include <cstdint>

namespace simdrpg::core {
    struct Vec2i{int x; int y;};
    struct Vec2f { float x, y; };
    struct Rect  { int x, y, w, h; };

    using Pixel = uint32_t;

    constexpr Pixel rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
        return (Pixel(a) << 24) | (Pixel(r) << 16) | (Pixel(g) << 8) | b;
    }
    constexpr uint8_t alpha_of(Pixel p) { return p >> 24; }
    constexpr uint8_t red_of  (Pixel p) { return (p >> 16) & 0xFF; }
    constexpr uint8_t green_of(Pixel p) { return (p >>  8) & 0xFF; }
    constexpr uint8_t blue_of (Pixel p) { return  p        & 0xFF; }

}

#endif //TOYPROJECTS_SIMDRPG_TYPES_H
