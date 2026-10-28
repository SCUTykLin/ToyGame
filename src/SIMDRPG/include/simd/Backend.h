//
// Created by 15813 on 2026-10-28.
//

#ifndef TOYPROJECTS_SIMDRPG_BACKEND_H
#define TOYPROJECTS_SIMDRPG_BACKEND_H

namespace simdrpg::simd {
    enum class Isa { Scalar, SSE2, AVX2 };

    Isa detect();
    Isa active();
    
    void blend_row_sse2(Pixel* dst, const Pixel* src, int n);
    void blend_row_avx2(Pixel* dst, const Pixel* src, int n);
    void blend_row_scalar(Pixel* dst, const Pixel* src, int n);

    inline void blend_row(Pixel* dst, const Pixel* src, int n) {
        switch (active()) {
            case Isa::AVX2:   return blend_row_avx2(dst, src, n);
            case Isa::SSE2:   return blend_row_sse2(dst, src, n);
            default:          return blend_row_scalar(dst, src, n);
        }
    }
}

#endif //TOYPROJECTS_SIMDRPG_BACKEND_H
