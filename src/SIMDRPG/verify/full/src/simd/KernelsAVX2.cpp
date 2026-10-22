// 这个文件必须单独开 /arch:AVX2（-mavx2），见 CMakeLists。
// 其他文件不能开，否则程序在没 AVX2 的机器上起不来。
#include "simd/Backend.h"

#include <immintrin.h>

namespace simd {

// 和 SSE2 版内核相同。这里重新写一份，是为了让这个翻译单元
// 不依赖 Kernels.cpp 里的 static inline 函数（跨 TU 内联不了）。
static inline __m128i blend4_core_avx(__m128i s, __m128i d) {
    const __m128i zero = _mm_setzero_si128();

    __m128i s_lo = _mm_unpacklo_epi8(s, zero);
    __m128i s_hi = _mm_unpackhi_epi8(s, zero);
    __m128i d_lo = _mm_unpacklo_epi8(d, zero);
    __m128i d_hi = _mm_unpackhi_epi8(d, zero);

    __m128i a_lo = _mm_shufflehi_epi16(_mm_shufflelo_epi16(s_lo, 0xFF), 0xFF);
    __m128i a_hi = _mm_shufflehi_epi16(_mm_shufflelo_epi16(s_hi, 0xFF), 0xFF);

    const __m128i k255 = _mm_set1_epi16(255);
    const __m128i k128 = _mm_set1_epi16(128);

    auto div255 = [k128](__m128i x) {
        const __m128i t = _mm_add_epi16(x, k128);
        const __m128i u = _mm_srli_epi16(t, 8);
        return _mm_srli_epi16(_mm_add_epi16(t, u), 8);
    };

    __m128i r_lo = _mm_add_epi16(
        div255(_mm_mullo_epi16(s_lo, a_lo)),
        div255(_mm_mullo_epi16(d_lo, _mm_sub_epi16(k255, a_lo))));
    __m128i r_hi = _mm_add_epi16(
        div255(_mm_mullo_epi16(s_hi, a_hi)),
        div255(_mm_mullo_epi16(d_hi, _mm_sub_epi16(k255, a_hi))));

    __m128i out = _mm_packus_epi16(r_lo, r_hi);
    return _mm_or_si128(out, _mm_set1_epi32(0xFF000000u));
}

// 一次 8 个像素。
//
// 注意：这里**没有**用到 256 位整数运算，是"两个 128 位半区各跑一遍"。
// 原因见下面注释——AVX2 的 unpack/pack 是按 128 位半区独立工作的，
// 直接写 256 位版本会把像素顺序搞乱。
void blend_row_avx2(gfx::Pixel* dst, const gfx::Pixel* src, int n) {
    int i = 0;
    for (; i + 8 <= n; i += 8) {
        __m128i s0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i + 0));
        __m128i d0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(dst + i + 0));
        __m128i s1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i + 4));
        __m128i d1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(dst + i + 4));

        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i + 0), blend4_core_avx(s0, d0));
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i + 4), blend4_core_avx(s1, d1));
    }
    // 尾部（n%8 个像素）
    blend_row_scalar(dst + i, src + i, n - i);
}

} // namespace simd
