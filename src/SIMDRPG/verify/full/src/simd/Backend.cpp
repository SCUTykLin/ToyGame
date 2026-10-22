#include "simd/Backend.h"

#include <intrin.h>

namespace simd {

namespace {

struct CpuInfo {
    bool sse2  = false;
    bool sse41 = false;
    bool avx2  = false;
};

CpuInfo probe_cpu() {
    CpuInfo info;
    int regs[4];

    __cpuid(regs, 0);
    const int max_id = regs[0];
    if (max_id < 1) return info;

    // CPUID leaf 1: EDX bit 26 = SSE2, ECX bit 19 = SSE4.1
    __cpuid(regs, 1);
    info.sse2  = (regs[3] & (1 << 26)) != 0;
    info.sse41 = (regs[2] & (1 << 19)) != 0;

    if (max_id >= 7) {
        // leaf 7 subleaf 0: EBX bit 5 = AVX2
        __cpuidex(regs, 7, 0);
        info.avx2 = (regs[1] & (1 << 5)) != 0;

        // 关键：光有 CPU 支持不够，操作系统必须已经打开 YMM 寄存器保存。
        // XCR0 的 bit1(SSE) 和 bit2(AVX) 都为 1 才行，否则 AVX 指令会崩。
        const unsigned long long xcr0 = _xgetbv(0);
        if ((xcr0 & 0x6) != 0x6) info.avx2 = false;
    }
    return info;
}

} // namespace

Isa detect() {
    // static 局部变量：只在第一次调用时执行，之后直接返回缓存值。
    // C++11 起这是线程安全的，不需要加锁。
    static const Isa isa = [] {
        const CpuInfo c = probe_cpu();
        if (c.avx2) return Isa::AVX2;
        if (c.sse2) return Isa::SSE2;
        return Isa::Scalar;
    }();
    return isa;
}

Isa active() { return detect(); }

const char* isa_name(Isa i) {
    switch (i) {
        case Isa::AVX2: return "AVX2";
        case Isa::SSE2: return "SSE2";
        default:        return "scalar";
    }
}

} // namespace simd
