//
// Created by 15813 on 2026-10-28.
// Blitter主要是利用SIMD混合像素，
//

#ifndef TOYPROJECTS_SIMDRPG_BLITTER_H
#define TOYPROJECTS_SIMDRPG_BLITTER_H

#include "gfx/Surface.h"

namespace simdrpg::core {
    // 源图不透明区域直接覆盖
    void blit_opaque  (Surface& dst, const Surface& src, Rect srcRect, int dx, int dy);
    // 带 alpha 混合：dst = src*a + dst*(1-a)
    void blit_alpha   (Surface& dst, const Surface& src, Rect srcRect, int dx, int dy);
    // 先给源图整体乘一个颜色（天气/光照/受击闪白）
    void blit_tinted  (Surface& dst, const Surface& src, Rect srcRect, int dx, int dy, Pixel tint);
    // 最近邻缩放，同意角色尺寸
    void blit_stretch (Surface& dst, const Surface& src, Rect srcRect, Rect dstRect);
    // 用源的 alpha 当遮罩、用 color 当颜色（忽略源 RGB）。
    // 字体、图标染色用它。和 blit_tinted 的区别：
    //   tinted 是"乘"，源本身是深色就永远涂不亮；
    //   mask   是"换"，只借形状。
    void blit_mask    (Surface& dst, const Surface& src, Rect srcRect, int dx, int dy, Pixel color);
    // 整块填色
    void fill_rect    (Surface& dst, Rect r, Pixel color);

    // 半透明整块填充
    void fill_rect_alpha(Surface& dst, Rect r, Pixel color);

    // 单像素混合
    void blend_pixel_scalar(Pixel& d, Pixel s);
}

#endif //TOYPROJECTS_SIMDRPG_BLITTER_H
