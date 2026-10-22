#include "gfx/Surface.h"

#include <cstdlib>
#include <cstring>
#include <utility>

#ifdef _MSC_VER
#include <malloc.h>     // _aligned_malloc / _aligned_free
#endif

namespace gfx {

// 向上取整到 a 的倍数。a 必须是 2 的幂。
static inline int align_up(int v, int a) { return (v + a - 1) & ~(a - 1); }

Surface::Surface(int w, int h) : w_(w), h_(h) {
    // 每行补到 8 像素 = 32 字节，正好是 AVX2 的对齐要求
    pitch_ = align_up(w, 8);

    const size_t bytes = size_t(pitch_) * size_t(h_) * sizeof(Pixel);

#ifdef _MSC_VER
    px_ = static_cast<Pixel*>(_aligned_malloc(bytes, 32));
#else
    void* p = nullptr;
    if (posix_memalign(&p, 32, bytes) != 0) p = nullptr;
    px_ = static_cast<Pixel*>(p);
#endif

    if (!px_) std::abort();
    std::memset(px_, 0, bytes);
}

Surface::~Surface() { release(); }

void Surface::release() {
    if (!px_) return;
#ifdef _MSC_VER
    _aligned_free(px_);      // 必须配 _aligned_malloc，不能用 free()
#else
    std::free(px_);
#endif
    px_ = nullptr;
}

// 移动：把源对象的指针"偷"过来，并把源置空
Surface::Surface(Surface&& o) noexcept
    : px_(std::exchange(o.px_, nullptr)),
      w_(std::exchange(o.w_, 0)),
      h_(std::exchange(o.h_, 0)),
      pitch_(std::exchange(o.pitch_, 0)) {}

Surface& Surface::operator=(Surface&& o) noexcept {
    if (this != &o) {
        release();                                  // 先放掉自己的
        px_    = std::exchange(o.px_, nullptr);
        w_     = std::exchange(o.w_, 0);
        h_     = std::exchange(o.h_, 0);
        pitch_ = std::exchange(o.pitch_, 0);
    }
    return *this;
}

void Surface::clear(Pixel color) {
    for (int y = 0; y < h_; ++y) {
        Pixel* r = row(y);
        for (int x = 0; x < w_; ++x) r[x] = color;
    }
}

} // namespace gfx
