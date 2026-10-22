#pragma once
#include <cstdint>
#include "core/Types.h"     // Rect / Pixel 的唯一来源

namespace gfx {

using core::Pixel;
using core::Rect;

// 一块自己管理的、32 字节对齐的像素缓冲。
//
// 不可拷贝、可移动。原因：它持有一块 malloc 来的内存，
// 拷贝会变成两个对象指向同一块内存，析构两次 → 崩溃。
class Surface {
public:
    // 默认构造：0×0，不分配内存。
    // 必须有。否则任何"含 Surface 成员"的类（TileMap / Game）都无法默认构造，
    // 报错是 C2280「尝试引用已删除的函数」，看不出跟 Surface 有关。
    Surface() = default;

    Surface(int w, int h);
    ~Surface();

    Surface(const Surface&) = delete;
    Surface& operator=(const Surface&) = delete;
    Surface(Surface&&) noexcept;
    Surface& operator=(Surface&&) noexcept;

    Pixel*       data()       { return px_; }
    const Pixel* data() const { return px_; }

    int width()  const { return w_; }
    int height() const { return h_; }
    int pitch()  const { return pitch_; }   // 每行像素数，≥ width

    // 取第 y 行行首。永远用这个，不要写 px_ + y * w_
    Pixel*       row(int y)       { return px_ + size_t(y) * size_t(pitch_); }
    const Pixel* row(int y) const { return px_ + size_t(y) * size_t(pitch_); }

    void clear(Pixel color);

private:
    void release();

    Pixel* px_ = nullptr;
    int w_ = 0, h_ = 0, pitch_ = 0;
};

} // namespace gfx
