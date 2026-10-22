#include "simd/Backend.h"

#include <immintrin.h>
#include <cstdint>

namespace simd {

// ---------------------------------------------------------------
// 1) 标量：黄金标准。SIMD 版对不对，靠它判断。
// ---------------------------------------------------------------
void blend_row_scalar(gfx::Pixel* dst, const gfx::Pixel* src, int n) {
    for (int i = 0; i < n; ++i) {
        const gfx::Pixel s = src[i];
        gfx::Pixel& d = dst[i];

        const uint32_t a = s >> 24;
        if (a == 0) continue;
        if (a == 255) { d = s; continue; }

        const uint32_t inv = 255 - a;
        const uint32_t sr = (s >> 16) & 0xFF, sg = (s >> 8) & 0xFF, sb = s & 0xFF;
        const uint32_t dr = (d >> 16) & 0xFF, dg = (d >> 8) & 0xFF, db = d & 0xFF;

        d = 0xFF000000u
          | (((sr * a + dr * inv) / 255) << 16)
          | (((sg * a + dg * inv) / 255) << 8)
          |  ((sb * a + db * inv) / 255);
    }
}

// ---------------------------------------------------------------
// 2) div255：把 x/255 算成 round(x/255)
//
//    (x + 128)
//    ---------  ≈  ((x + 128) + ((x + 128) >> 8)) >> 8
//       255
//
//    第一步加 128 = 四舍五入；后两步是 "除以 255 用除以 256 逼近"。
//    误差：对 x ∈ [0, 65025] 最大偏差 1，且偏的方向固定（比 floor 大 1）。
//
//    这就是黄金测试必须容忍 ±1 的原因。
// ---------------------------------------------------------------
static inline __m128i div255_epi16(__m128i x) {
    const __m128i k128 = _mm_set1_epi16(128);
    const __m128i t = _mm_add_epi16(x, k128);
    const __m128i u = _mm_srli_epi16(t, 8);
    return _mm_srli_epi16(_mm_add_epi16(t, u), 8);
}

// 一次处理 4 个像素（16 字节）
static inline __m128i blend4_core(__m128i s, __m128i d) {
    const __m128i zero = _mm_setzero_si128();

    // 8 位 → 16 位，否则乘法会溢出。
    // 低 8 字节（前 2 个像素）和高 8 字节（后 2 个像素）分开处理。
    __m128i s_lo = _mm_unpacklo_epi8(s, zero);   // [B0 G0 R0 A0 | B1 G1 R1 A1]
    __m128i s_hi = _mm_unpackhi_epi8(s, zero);   // [B2 G2 R2 A2 | B3 G3 R3 A3]
    __m128i d_lo = _mm_unpacklo_epi8(d, zero);
    __m128i d_hi = _mm_unpackhi_epi8(d, zero);

    // 把每个像素的 A 广播到该像素的 4 个通道。
    // 0xFF 是"复制第 3 个 word 到全部 4 个 word"，lo/hi 各一次覆盖 4 个 word。
    __m128i a_lo = _mm_shufflehi_epi16(_mm_shufflelo_epi16(s_lo, 0xFF), 0xFF);
    __m128i a_hi = _mm_shufflehi_epi16(_mm_shufflelo_epi16(s_hi, 0xFF), 0xFF);

    const __m128i k255 = _mm_set1_epi16(255);

    // 关键：先除再加。
    // s*a 最大 255*255 = 65025，d*inv 也是 65025，
    // 直接相加 130050 会溢出 16 位（上限 65535）。
    // 分开除之后每个 ≤255，加起来 ≤510，安全。
    __m128i r_lo = _mm_add_epi16(
        div255_epi16(_mm_mullo_epi16(s_lo, a_lo)),
        div255_epi16(_mm_mullo_epi16(d_lo, _mm_sub_epi16(k255, a_lo))));

    __m128i r_hi = _mm_add_epi16(
        div255_epi16(_mm_mullo_epi16(s_hi, a_hi)),
        div255_epi16(_mm_mullo_epi16(d_hi, _mm_sub_epi16(k255, a_hi))));

    // 16 位 → 8 位（带饱和，>255 截成 255，负数截成 0）
    __m128i out = _mm_packus_epi16(r_lo, r_hi);

    // 强制不透明。alpha 已经在混合里用掉了，输出不需要它。
    return _mm_or_si128(out, _mm_set1_epi32(0xFF000000u));
}

void blend_row_sse2(gfx::Pixel* dst, const gfx::Pixel* src, int n) {
    int i = 0;
    for (; i + 4 <= n; i += 4) {
        __m128i s = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i));
        __m128i d = _mm_loadu_si128(reinterpret_cast<const __m128i*>(dst + i));
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + i), blend4_core(s, d));
    }
    // 尾部 0–3 个像素交给标量
    for (; i < n; ++i) {
        const gfx::Pixel s = src[i];
        gfx::Pixel& d = dst[i];
        const uint32_t a = s >> 24;
        if (a == 0) continue;
        if (a == 255) { d = s; continue; }
        const uint32_t inv = 255 - a;
        const uint32_t sr = (s >> 16) & 0xFF, sg = (s >> 8) & 0xFF, sb = s & 0xFF;
        const uint32_t dr = (d >> 16) & 0xFF, dg = (d >> 8) & 0xFF, db = d & 0xFF;
        d = 0xFF000000u
          | (((sr * a + dr * inv) / 255) << 16)
          | (((sg * a + dg * inv) / 255) << 8)
          |  ((sb * a + db * inv) / 255);
    }
}

} // namespace simd
