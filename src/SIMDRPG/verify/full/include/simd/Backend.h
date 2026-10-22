#pragma once
#include <cstdint>
#include "gfx/Surface.h"

namespace simd {

enum class Isa { Scalar, SSE2, AVX2 };

Isa detect();                    // 探测一次，缓存
Isa active();                    // 当前生效的
const char* isa_name(Isa i);

// 三个后端，签名完全一样
void blend_row_scalar(gfx::Pixel* d, const gfx::Pixel* s, int n);
void blend_row_sse2  (gfx::Pixel* d, const gfx::Pixel* s, int n);
void blend_row_avx2  (gfx::Pixel* d, const gfx::Pixel* s, int n);

// 唯一入口。Blitter 只调这个，不关心底层是谁。
inline void blend_row(gfx::Pixel* d, const gfx::Pixel* s, int n) {
    switch (active()) {
        case Isa::AVX2: return blend_row_avx2(d, s, n);
        case Isa::SSE2: return blend_row_sse2(d, s, n);
        default:        return blend_row_scalar(d, s, n);
    }
}

} // namespace simd
