// SIMD 讲义代码验证器
// 把 docs/02-SIMD从零到能用.md 里的代码原样搬过来，
// 用标量版当黄金标准，验证 SSE2 / AVX2 版是否一致。
// 编译: cl /O2 /arch:AVX2 /EHsc verify_simd.cpp
#include <immintrin.h>
#include <cstdint>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cstring>

using Pixel = uint32_t;

// ---------- 6.2 标量版（黄金标准）----------
static void blend_pixel_scalar(Pixel& d, Pixel s) {
    const uint32_t a = s >> 24;
    if (a == 0) return;
    if (a == 255) { d = s; return; }
    const uint32_t inv = 255 - a;
    const uint32_t sr = (s >> 16) & 0xFF, sg = (s >> 8) & 0xFF, sb = s & 0xFF;
    const uint32_t dr = (d >> 16) & 0xFF, dg = (d >> 8) & 0xFF, db = d & 0xFF;
    d = 0xFF000000u
      | (((sr * a + dr * inv) / 255) << 16)
      | (((sg * a + dg * inv) / 255) << 8)
      |  ((sb * a + db * inv) / 255);
}

// ---------- 6.3 除法技巧 ----------
static inline uint32_t div255_scalar(uint32_t x) {
    uint32_t t = x + 128;
    return (t + (t >> 8)) >> 8;
}

// ---------- 6.5 SSE2 ----------
static inline __m128i div255_epi16(__m128i x) {
    const __m128i k128 = _mm_set1_epi16(128);
    __m128i t = _mm_add_epi16(x, k128);
    __m128i u = _mm_srli_epi16(t, 8);
    return _mm_srli_epi16(_mm_add_epi16(t, u), 8);
}

static void blend4_sse2(Pixel* dst, const Pixel* src) {
    const __m128i zero = _mm_setzero_si128();
    __m128i s = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src));
    __m128i d = _mm_loadu_si128(reinterpret_cast<const __m128i*>(dst));

    __m128i s_lo = _mm_unpacklo_epi8(s, zero);
    __m128i s_hi = _mm_unpackhi_epi8(s, zero);
    __m128i d_lo = _mm_unpacklo_epi8(d, zero);
    __m128i d_hi = _mm_unpackhi_epi8(d, zero);

    __m128i a_lo = _mm_shufflehi_epi16(_mm_shufflelo_epi16(s_lo, 0xFF), 0xFF);
    __m128i a_hi = _mm_shufflehi_epi16(_mm_shufflelo_epi16(s_hi, 0xFF), 0xFF);

    const __m128i k255 = _mm_set1_epi16(255);
    __m128i inv_lo = _mm_sub_epi16(k255, a_lo);
    __m128i inv_hi = _mm_sub_epi16(k255, a_hi);

    __m128i r_lo = _mm_add_epi16(
        div255_epi16(_mm_mullo_epi16(s_lo, a_lo)),
        div255_epi16(_mm_mullo_epi16(d_lo, inv_lo)));
    __m128i r_hi = _mm_add_epi16(
        div255_epi16(_mm_mullo_epi16(s_hi, a_hi)),
        div255_epi16(_mm_mullo_epi16(d_hi, inv_hi)));

    __m128i out = _mm_packus_epi16(r_lo, r_hi);
    out = _mm_or_si128(out, _mm_set1_epi32(0xFF000000u));
    _mm_storeu_si128(reinterpret_cast<__m128i*>(dst), out);
}

static void blend_row_sse2(Pixel* dst, const Pixel* src, int n) {
    int i = 0;
    for (; i + 4 <= n; i += 4) blend4_sse2(dst + i, src + i);
    for (; i < n; ++i) blend_pixel_scalar(dst[i], src[i]);
}

// ---------- 6.6 AVX2（讲义原版，怀疑顺序有问题）----------
static inline __m256i div255_epi16_256(__m256i x) {
    const __m256i k128 = _mm256_set1_epi16(128);
    __m256i t = _mm256_add_epi16(x, k128);
    __m256i u = _mm256_srli_epi16(t, 8);
    return _mm256_srli_epi16(_mm256_add_epi16(t, u), 8);
}

static inline __m256i broadcast_alpha_256(__m256i v) {
    __m128i lo = _mm256_castsi256_si128(v);
    __m128i hi = _mm256_extracti128_si256(v, 1);
    lo = _mm_shufflehi_epi16(_mm_shufflelo_epi16(lo, 0xFF), 0xFF);
    hi = _mm_shufflehi_epi16(_mm_shufflelo_epi16(hi, 0xFF), 0xFF);
    return _mm256_set_m128i(hi, lo);
}

// 讲义里的原始写法
static void blend8_avx2_lecture(Pixel* dst, const Pixel* src) {
    const __m256i zero = _mm256_setzero_si256();
    __m256i s = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src));
    __m256i d = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(dst));

    __m256i s16  = _mm256_unpacklo_epi8(s, zero);
    __m256i s16h = _mm256_unpackhi_epi8(s, zero);
    __m256i d16  = _mm256_unpacklo_epi8(d, zero);
    __m256i d16h = _mm256_unpackhi_epi8(d, zero);

    __m256i a16  = broadcast_alpha_256(s16);
    __m256i a16h = broadcast_alpha_256(s16h);

    const __m256i k255 = _mm256_set1_epi16(255);
    __m256i r1 = _mm256_add_epi16(
        div255_epi16_256(_mm256_mullo_epi16(s16,  a16)),
        div255_epi16_256(_mm256_mullo_epi16(d16, _mm256_sub_epi16(k255, a16))));
    __m256i r2 = _mm256_add_epi16(
        div255_epi16_256(_mm256_mullo_epi16(s16h,  a16h)),
        div255_epi16_256(_mm256_mullo_epi16(d16h, _mm256_sub_epi16(k255, a16h))));

    __m256i out = _mm256_packus_epi16(r1, r2);
    out = _mm256_permute4x64_epi64(out, 0xD8);
    out = _mm256_or_si256(out, _mm256_set1_epi32(0xFF000000u));
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(dst), out);
}

// ===== 以下与 docs/02-SIMD从零到能用.md 6.6 节修正版逐字一致 =====
static inline __m128i blend4_core(__m128i s, __m128i d) {
    const __m128i zero = _mm_setzero_si128();
    __m128i s_lo = _mm_unpacklo_epi8(s, zero);
    __m128i s_hi = _mm_unpackhi_epi8(s, zero);
    __m128i d_lo = _mm_unpacklo_epi8(d, zero);
    __m128i d_hi = _mm_unpackhi_epi8(d, zero);

    __m128i a_lo = _mm_shufflehi_epi16(_mm_shufflelo_epi16(s_lo, 0xFF), 0xFF);
    __m128i a_hi = _mm_shufflehi_epi16(_mm_shufflelo_epi16(s_hi, 0xFF), 0xFF);

    const __m128i k255 = _mm_set1_epi16(255);
    __m128i r_lo = _mm_add_epi16(
        div255_epi16(_mm_mullo_epi16(s_lo, a_lo)),
        div255_epi16(_mm_mullo_epi16(d_lo, _mm_sub_epi16(k255, a_lo))));
    __m128i r_hi = _mm_add_epi16(
        div255_epi16(_mm_mullo_epi16(s_hi, a_hi)),
        div255_epi16(_mm_mullo_epi16(d_hi, _mm_sub_epi16(k255, a_hi))));

    __m128i out = _mm_packus_epi16(r_lo, r_hi);
    return _mm_or_si128(out, _mm_set1_epi32(0xFF000000u));
}

static void blend8_avx2_fixed(Pixel* dst, const Pixel* src) {
    __m128i s_lo = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + 0));
    __m128i d_lo = _mm_loadu_si128(reinterpret_cast<const __m128i*>(dst + 0));
    __m128i s_hi = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + 4));
    __m128i d_hi = _mm_loadu_si128(reinterpret_cast<const __m128i*>(dst + 4));

    _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + 0), blend4_core(s_lo, d_lo));
    _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + 4), blend4_core(s_hi, d_hi));
}

static void blend_row_avx2_fixed(Pixel* dst, const Pixel* src, int n) {
    int i = 0;
    for (; i + 8 <= n; i += 8) blend8_avx2_fixed(dst + i, src + i);
    for (; i < n; ++i) blend_pixel_scalar(dst[i], src[i]);
}

// ---------- 测试 ----------
static int max_diff(Pixel a, Pixel b) {
    int dr = std::abs(int((a >> 16) & 0xFF) - int((b >> 16) & 0xFF));
    int dg = std::abs(int((a >> 8)  & 0xFF) - int((b >> 8)  & 0xFF));
    int db = std::abs(int( a        & 0xFF) - int( b        & 0xFF));
    int m = dr > dg ? dr : dg;
    return m > db ? m : db;
}

static void dump(const char* tag, const Pixel* p, int n) {
    printf("%s:", tag);
    for (int i = 0; i < n; ++i) printf(" %08X", p[i]);
    printf("\n");
}

int main() {
    printf("=== SIMD 讲义代码验证 ===\n\n");

    // 1) div255 语义验证
    //    讲义里说它是 round(x/255)，而 C++ 的 x/255 是 floor。
    //    这里验证两件事：(a) 它确实等于四舍五入 (b) 与 floor 的差不超过 1
    int div_bad = 0, div_floor_diff = 0, div_maxdiff = 0;
    for (uint32_t x = 0; x <= 65025u; ++x) {
        uint32_t got  = div255_scalar(x);
        uint32_t want = (x + 127) / 255;          // 四舍五入的参考实现
        if (got != want) { if (div_bad < 5) printf("  div255(%u)=%u 应为 %u\n", x, got, want); ++div_bad; }
        uint32_t fl = x / 255;                    // 截断
        int d = int(got) - int(fl);
        if (d < 0) d = -d;
        if (d > div_maxdiff) div_maxdiff = d;
        if (d != 0) ++div_floor_diff;
    }
    printf("[1] div255 语义 (0..65025):\n");
    printf("    等于 round(x/255): %s (%d 处不符)\n", div_bad ? "失败" : "通过", div_bad);
    printf("    与 floor(x/255) 最大差 %d，%d 处不同（四舍五入 vs 截断，预期内）\n\n",
           div_maxdiff, div_floor_diff);

    // 2) SSE2 vs 标量
    std::srand(12345);
    int sse_bad = 0, sse_max = 0;
    for (int iter = 0; iter < 20000; ++iter) {
        Pixel src[16], d0[16], d1[16];
        for (int i = 0; i < 16; ++i) {
            src[i] = (std::rand() << 16) ^ std::rand();
            d0[i] = d1[i] = (std::rand() << 16) ^ std::rand();
        }
        for (int i = 0; i < 16; ++i) blend_pixel_scalar(d0[i], src[i]);
        blend_row_sse2(d1, src, 16);
        for (int i = 0; i < 16; ++i) {
            int df = max_diff(d0[i], d1[i]);
            if (df > sse_max) sse_max = df;
            if (df > 1) ++sse_bad;
        }
    }
    printf("[2] SSE2 vs 标量: %s (最大误差 %d, 超容 %d)\n", sse_bad ? "失败" : "通过", sse_max, sse_bad);

    // 3) AVX2 讲义原版
    {
        Pixel src[8], d0[8], d1[8];
        for (int i = 0; i < 8; ++i) {
            src[i] = 0x80000000u | (i * 37u);          // alpha=0x80, 颜色递增便于看顺序
            d0[i] = d1[i] = 0xFF102030u;
        }
        for (int i = 0; i < 8; ++i) blend_pixel_scalar(d0[i], src[i]);
        blend8_avx2_lecture(d1, src);
        printf("[3] AVX2 讲义原版:\n");
        dump("      标量", d0, 8);
        dump("      AVX2", d1, 8);
        int bad = 0; for (int i = 0; i < 8; ++i) if (max_diff(d0[i], d1[i]) > 1) ++bad;
        printf("      结果: %s (%d/8 不符)\n\n", bad ? "失败 — 讲义代码有 bug" : "通过", bad);
    }

    // 4) AVX2 修正版
    {
        int bad = 0, mx = 0;
        for (int iter = 0; iter < 20000; ++iter) {
            Pixel src[16], d0[16], d1[16];
            for (int i = 0; i < 16; ++i) {
                src[i] = (std::rand() << 16) ^ std::rand();
                d0[i] = d1[i] = (std::rand() << 16) ^ std::rand();
            }
            for (int i = 0; i < 16; ++i) blend_pixel_scalar(d0[i], src[i]);
            blend_row_avx2_fixed(d1, src, 16);
            for (int i = 0; i < 16; ++i) {
                int df = max_diff(d0[i], d1[i]);
                if (df > mx) mx = df;
                if (df > 1) ++bad;
            }
        }
        printf("[4] AVX2 修正版: %s (最大误差 %d, 超容 %d)\n\n", bad ? "失败" : "通过", mx, bad);
    }

    // 5) 尾部处理（n 不是 4/8 的倍数）
    {
        int bad = 0;
        for (int n = 1; n <= 17; ++n) {
            Pixel src[32], d0[32], d1[32], d2[32];
            for (int i = 0; i < 32; ++i) {
                src[i] = (std::rand() << 16) ^ std::rand();
                d0[i] = d1[i] = d2[i] = (std::rand() << 16) ^ std::rand();
            }
            for (int i = 0; i < n; ++i) blend_pixel_scalar(d0[i], src[i]);
            blend_row_sse2(d1, src, n);
            blend_row_avx2_fixed(d2, src, n);
            for (int i = 0; i < n; ++i)
                if (max_diff(d0[i], d1[i]) > 1 || max_diff(d0[i], d2[i]) > 1) ++bad;
        }
        printf("[5] 尾部处理 n=1..17: %s\n\n", bad ? "失败" : "通过");
    }

    return 0;
}
