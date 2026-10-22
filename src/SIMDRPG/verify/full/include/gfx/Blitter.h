#pragma once
#include "gfx/Surface.h"

namespace gfx {

// 注意：Rect 来自 Surface.h -> core/Types.h。
// 不要在这里再定义一次 struct Rect。

// 不透明拷贝：源区域直接覆盖，忽略 alpha
void blit_opaque(Surface& dst, const Surface& src, Rect r, int dx, int dy);

// alpha 混合：dst = src*a + dst*(1-a)
void blit_alpha(Surface& dst, const Surface& src, Rect r, int dx, int dy);

// 先给源整体乘一个颜色，再混合。用于受击闪白、天气染色
void blit_tinted(Surface& dst, const Surface& src, Rect r, int dx, int dy, Pixel tint);

// 用 alpha 当遮罩、用 color 当颜色绘制。
//
// 和 blit_tinted 的区别：
//   blit_tinted = 源像素 × tint   （乘法染色，源是黑的永远涂不亮）
//   blit_mask   = 源alpha + color （忽略源 RGB，只借它的形状）
//
// 字体必须用后者。点阵字体图集里的字形常常不是白色
// （我们这套 font8x8.png 是 11,0,30 的深蓝紫），
// 用 tinted 去乘，白字乘出来还是深色 —— 结果就是"文字画了但看不见"。
void blit_mask(Surface& dst, const Surface& src, Rect r, int dx, int dy, Pixel color);

// 整块填色（UI 背景、雨天压暗）
// 注意：这是**直接写像素**，不看 alpha。要半透明见下面那个。
void fill_rect(Surface& dst, Rect r, Pixel color);

// 半透明整块填充：逐像素走 alpha 混合。
// 雾、暗角、编辑器的网格高亮都靠它。
// 用 fill_rect 画雾的后果是整屏变成一块纯色板 —— 这个坑踩过一次。
void fill_rect_alpha(Surface& dst, Rect r, Pixel color);

// 最近邻缩放绘制：把源矩形 s 拉伸到目标矩形 d。
// 怪物素材尺寸五花八门（13×10 到 906×420），靠这个统一到目标框。
// 用最近邻而不是双线性，是为了保持像素画的硬边。
void blit_stretch(Surface& dst, const Surface& src, Rect s, Rect d);

// 单像素混合，给 UI / 特效 / 雨滴用
void blend_pixel_scalar(Pixel& d, Pixel s);

} // namespace gfx
