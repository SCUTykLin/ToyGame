# 02 · SIMD 从零到能用

> 这一章单独啃，别和别的章节混着看。
> 目标：读完之后你能独立写出「一次处理 8 个像素」的 alpha 混合，并且**能证明它比标量快**。

---

## 0. 先算一笔账

你的屏幕 1280×720 = **921,600 像素**，60 FPS 就是每秒 5500 万像素。

每个像素做一次 alpha 混合，标量代码大约这些指令：
```
读 src、读 dst、拆出 a、算 255-a、3 个通道各两次乘加、除 255、写回
≈ 25~40 条指令/像素
```

按 30 条算：`921600 × 30 × 60 ≈ 16.6 亿条指令/秒`。
现代 CPU 单核约 30~50 亿条/秒，看起来还行 —— 但这是**只做混合、别的什么都不干**的情况。
你还要算地图、算 AI、算粒子、算天气。

所以真实的瓶颈在这儿：**每像素独立计算，是纯串行浪费**。

SIMD 的思路一句话：**CPU 的寄存器本来就比 32 位宽得多，你一直只用其中 1/4 或 1/8。**

---

## 1. 心智模型：寄存器是一排格子

### 1.1 标量 vs 向量

```
标量运算（你现在写的）：
   eax: [ 0x11223344 ]
   ebx: [ 0x55667788 ]
    add → 1 次加法，得 1 个结果

SSE2（128 位）：
   xmm0: [ p0 | p1 | p2 | p3 ]      ← 4 个 32 位像素
   xmm1: [ q0 | q1 | q2 | q3 ]
   paddd → 1 次加法，得 4 个结果

AVX2（256 位）：
   ymm0: [ p0 | p1 | p2 | p3 | p4 | p5 | p6 | p7 ]   ← 8 个
   vpaddd → 1 次加法，得 8 个结果
```

**关键认知**：不是「并行执行 8 条指令」（那是多核/超线程），
而是「**一条指令同时作用于 8 个数据**」（SIMD = Single Instruction, Multiple Data）。

这些格子在术语里叫 **lane（通道）**。

### 1.2 同一个 128 位寄存器，能有几种看法

这是最容易懵的地方。128 位是固定的，但你可以把它解释成不同粒度：

| 类型 | 类型名 | lane 数量 | 每个 lane 宽度 |
|------|--------|----------|---------------|
| 16 个字节 | `__m128i` | 16 | int8 |
| 8 个短整 | `__m128i` | 8 | int16 |
| 4 个整型 | `__m128i` | 4 | int32 |
| 4 个单精度 | `__m128` | 4 | float |

**物理上完全一样**，只是你用不同指令去解读。

> 这就是为什么 alpha 混合能做到：先把 4 个 `uint32` 像素「看成」16 个字节，
> 才能把 R/G/B/A 拆开单独算。

### 1.3 指令命名规律

Intel intrinsics 的命名是有规则的，记住这个你就能猜出 80% 的函数：

```
_mm_   <操作>  _<数据类型>
 ↑        ↑         ↑
前缀   干什么    对什么数据

_mm_add_epi32       加     有符号 32 位整数
_mm_mullo_epi16     乘(低16) 有符号 16 位
_mm_mulhi_epu16     乘(高16) 无符号 16 位
_mm_srli_epi32      右移(逻辑) 32 位
_mm_and_si128       按位与  任意
_mm_set1_epi32      广播
_mm_load_si128      对齐加载
_mm_loadu_si128     非对齐加载
_mm_store_si128     对齐存储
_mm_unpacklo_epi8   交错展开(低位)
_mm_packus_epi16    饱和窄化到无符号 8 位
_mm_shufflelo_epi16 低 4 个 16 位通道重排
```

`p` 前缀 = packed（打包，所有 lane 一起算）。
反例 `_mm_add_ss` 里的 `ss` = scalar single（只算最低那个 lane），那种是给浮点串行用的，别用错。

AVX2 就把 `_mm_` 换成 `_mm256_`，lane 数翻倍，其余规则一样。

---

## 2. 准备环境

### 2.1 头文件

```cpp
#include <immintrin.h>   // 全部 SSE/AVX intrinsics
```

MSVC / GCC / Clang 都支持，不需要额外链接库。

### 2.2 编译选项

**MSVC**（你的环境）：
```cmake
# SSE2 是 x64 的默认基线，不用额外开
# AVX2 要用运行时检测，所以不能全局开 /arch:AVX2，
# 而是给"用 AVX2 的那个 .cpp"单独加：
set_source_files_properties(src/simd/KernelsAVX2.cpp PROPERTIES
    COMPILE_OPTIONS "/arch:AVX2")
```

> **为什么不能全局开 `/arch:AVX2`？**
> 因为开了以后编译器可能在任何地方生成 AVX2 指令，
> 而你的 CPU 支持不代表别人的 CPU 支持。全局开 = 程序在老机器上直接崩（非法指令）。
> 正确做法：**只有那个专门的文件用 AVX2 指令，运行时先 CPUID 检测再决定调不调**。

**GCC / Clang**：
```cmake
set_source_files_properties(src/simd/KernelsAVX2.cpp PROPERTIES
    COMPILE_OPTIONS "-mavx2")
```

### 2.3 验证你的 CPU 支持什么

写个 10 行的探测程序先跑一遍：

```cpp
#if defined(_MSC_VER)
#include <intrin.h>
#endif
#include <cstdio>

int main() {
#if defined(_MSC_VER)
    // MSVC 没有 __builtin_cpu_supports，要用 __cpuid 手查
    int regs[4];
    __cpuid(regs, 0);
    const int max_id = regs[0];

    __cpuid(regs, 1);
    const bool sse2  = (regs[3] & (1 << 26)) != 0;
    const bool sse41 = (regs[2] & (1 << 19)) != 0;

    bool avx2 = false;
    if (max_id >= 7) {
        __cpuidex(regs, 7, 0);
        avx2 = (regs[1] & (1 << 5)) != 0;
        // 光 CPU 支持不够，操作系统必须已打开 YMM 保存
        if ((_xgetbv(0) & 0x6) != 0x6) avx2 = false;
    }
#else
    __builtin_cpu_init();
    const bool sse2  = __builtin_cpu_supports("sse2");
    const bool sse41 = __builtin_cpu_supports("sse4.1");
    const bool avx2  = __builtin_cpu_supports("avx2");
#endif
    std::printf("SSE2=%d  SSE4.1=%d  AVX2=%d\n", sse2, sse41, avx2);
    return 0;
}
```

**两个平台各走一条路**：MSVC 用 `__cpuid` / `_xgetbv`，GCC/Clang 用
`__builtin_cpu_supports`。上面这份两边都能编译——这正是第 8 节 `Backend.cpp`
里 `probe_cpu()` 的雏形。

> 提示：2015 年之后的 Intel/AMD 桌面 CPU 基本都有 AVX2。
> 但笔记本低压 U、部分老 Xeon 没有。**永远要留标量兜底**。

---

## 3. 热身：数组加法

先建立手感，别看不起这个例子。

### 3.1 标量版

```cpp
void add_scalar(float* a, const float* b, int n) {
    for (int i = 0; i < n; ++i)
        a[i] += b[i];
}
```

### 3.2 SSE2 版（4 个一批）

```cpp
void add_sse(float* a, const float* b, int n) {
    int i = 0;
    for (; i + 4 <= n; i += 4) {
        __m128 va = _mm_loadu_ps(a + i);      // 读 4 个 float
        __m128 vb = _mm_loadu_ps(b + i);
        _mm_storeu_ps(a + i, _mm_add_ps(va, vb));
    }
    for (; i < n; ++i) a[i] += b[i];          // 尾巴
}
```

### 3.3 AVX2 版（8 个一批）

```cpp
void add_avx(float* a, const float* b, int n) {
    int i = 0;
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(a + i);
        __m256 vb = _mm256_loadu_ps(b + i);
        _mm256_storeu_ps(a + i, _mm256_add_ps(va, vb));
    }
    for (; i < n; ++i) a[i] += b[i];
}
```

**三个必须记住的细节**：

1. **尾部循环不能省**。`n` 不一定是 4 或 8 的倍数。省了就是越界读写。
2. **`i + 4 <= n` 不是 `i < n`**。写成 `i < n` 会在最后一次越界。
3. **`loadu` 的 `u` = unaligned**。对齐的用 `load`（更快），但用错了会直接崩。

---

## 4. 内存对齐：最容易崩的地方

### 4.1 为什么要求对齐

CPU 读内存是一块一块读的（cache line 64 字节）。
如果一次 16 字节的读跨越了 cache line 边界，硬件得读两次再拼——慢，而且某些指令干脆不允许。

`_mm_load_si128`（对齐版）要求地址是 **16 的倍数**，
`_mm256_load_si256` 要求 **32 的倍数**。
不满足 → **段错误 / 访问违例**，不是"慢一点"，是直接崩。

### 4.2 怎么保证

**方案 A：让数据自己对齐**

```cpp
struct alignas(32) Surface {
    uint32_t* pixels;
    int w, h, pitch;
};

// 分配
#ifdef _MSC_VER
    px_ = static_cast<uint32_t*>(_aligned_malloc(bytes, 32));
#else
    void* p = nullptr;
    posix_memalign(&p, 32, bytes);
    px_ = static_cast<uint32_t*>(p);
#endif
```

还要保证 **每行起始地址也对齐**：
`pitch`（每行像素数）必须是 8 的倍数，这样 `row(y) = base + y*pitch` 永远是 32 字节对齐。

```cpp
pitch_ = (w + 7) & ~7;   // 向上取整到 8 的倍数
```

> 这就是为什么 `Surface` 的 `pitch` 可能大于 `width`。
> 渲染时一定要用 `pitch` 算行偏移，**不能用 `w * y`**。这个 bug 特别隐蔽：
> 只有当 `w` 不是 8 的倍数时才出错，而你平时测试的地图宽度很可能是 64（正好是 8 的倍数）。

**方案 B：不想管对齐就用 `loadu`**

```cpp
_mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr))    // 任意地址
_mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr))
```

代价：在老 CPU 上可能慢 10~20%，新 CPU（Haswell 之后）基本没差别。

**本项目策略**：
- `Surface` 的分配**强制 32 字节对齐**
- 精灵的源图**不保证对齐**，用 `loadu`
- 目标缓冲**按行对齐**，用 `store`

---

## 5. 打包整数运算的坑

这是 SIMD 里第二个高频崩溃区。

### 5.1 `_mm_mullo_epi16` 会溢出

它算的是 16 位有符号乘法的**低 16 位**：

```cpp
// 255 * 255 = 65025
// 65025 放不进 int16（最大 32767），但放得进 uint16（最大 65535）
// 所以"255*255"这种刚好的情况没问题，但 255*256 = 65280 也没问题，
// 一旦两个数都接近 255 且是"有符号"解释就会翻车。
```

**规则**：所有像素运算一律当 **uint16（无符号）** 处理，
且保证乘积 ≤ 65535。`255 × 255 = 65025` 刚好安全，这是设计好的巧合。

### 5.2 两个乘积相加一定溢出

```
s*a       ≤ 65025
d*(255-a) ≤ 65025
两者相加   ≤ 130050   ← 超过 uint16 了
```

**所以不能直接加**。第 6 节会给规避办法（分别除 255 再相加）。

### 5.3 饱和运算指令

有些指令是「饱和」的：超出范围就钳到边界，而不是回绕。

```cpp
_mm_adds_epu8(a, b)    // 无符号 8 位饱和加：255 + 1 = 255（不是 0）
_mm_subs_epu8(a, b)    // 无符号 8 位饱和减：0 - 1 = 0（不是 255）
_mm_packus_epi16(a,b)  // 16 位 → 8 位，负数变 0，大于 255 变 255
```

`packus` 这个尤其重要：它是**把 16 位中间结果窄化回 8 位的标准手段**，
自带 clamp，不用担心溢出。

---

## 6. Alpha 混合：从公式到指令

这是本项目的核心。慢慢看，每一行都要懂。

### 6.1 数学

非预乘 alpha 的标准公式（每个颜色通道独立）：

```
out_c = (src_c × a + dst_c × (255 − a)) / 255
```

其中 `a` 是源像素的 alpha（0~255）。手工验证：
- `a = 255`：`out = (src×255 + 0)/255 = src` ✓ 完全不透明
- `a = 0`：  `out = (0 + dst×255)/255 = dst` ✓ 完全透明

### 6.2 标量实现（先写对，别管快慢）

```cpp
inline void blend_pixel_scalar(uint32_t& d, uint32_t s) {
    const uint32_t a = s >> 24;              // 源 alpha
    if (a == 255) { d = s; return; }         // 快速路径：不透明直接覆盖
    if (a == 0)   { return; }               // 完全透明，跳过

    const uint32_t inv = 255 - a;

    // 逐通道
    const uint32_t sr = (s >> 16) & 0xFF, sg = (s >> 8) & 0xFF, sb = s & 0xFF;
    const uint32_t dr = (d >> 16) & 0xFF, dg = (d >> 8) & 0xFF, db = d & 0xFF;

    const uint32_t r = (sr * a + dr * inv) / 255;
    const uint32_t g = (sg * a + dg * inv) / 255;
    const uint32_t b = (sb * a + db * inv) / 255;

    d = 0xFF000000u | (r << 16) | (g << 8) | b;
}
```

> 这里有个**重要事实**：LPC 的精灵边缘有大量半透明像素，
> 但大片内部是 `a == 255`。所以那个快速路径不是微优化，是**真的省一半时间**。
> 不过 SIMD 版没法这么便宜地跳过，所以 SIMD 的收益会体现在「整行都要混」的情况，
> 比如地面层叠加雾效、光晕、粒子。

### 6.3 除法：不要真的除

`/255` 在 SIMD 里没有对应指令（整数除法本身就慢，向量整数除法更是慢到离谱）。

关键技巧 —— 一个整数除 255 的精确近似：

```cpp
inline uint32_t div255(uint32_t x) {
    uint32_t t = x + 128;
    return (t + (t >> 8)) >> 8;
}
```

**它算的到底是什么**：
设 `x = 255q + r`，其中 `0 ≤ r < 255`。
- `r ≤ 127` 时算出 `q`
- `r ≥ 128` 时算出 `q + 1`

也就是 **round(x / 255)** —— 四舍五入。

### ⚠️ 它和 C++ 的 `x / 255` 不是一回事

整数除法是**向下取整**：`128 / 255 == 0`，但 `div255(128) == 1`。

这不是笔误，是**我在审核时实测出来的**（跑 `tools/verify_simd.cpp`）：
把 `x` 从 0 到 65025 全跑一遍，**有 32385 个值和 `x / 255` 不相等**，
全部恰好差 1，全部源于"四舍五入 vs 截断"。

后果：**SIMD 版和标量版的混合结果会有 ±1/255 的差异**。
这不是 bug，1/255 ≈ 0.4% 的亮度差，肉眼绝对看不出来。
但它正是 6.7 节"黄金测试必须容忍误差 1"的**真正原因**。

> **想让两边逐位一致**：把标量版也改成用 `div255` 而不是 `/ 255`，
> 这样两边都是四舍五入，误差直接变 0。代价是标量版慢一点点。
>
> ```cpp
> // 两种写法，选一个，但两边必须一致
> const uint32_t r = (sr * a + dr * inv) / 255;         // 截断
> const uint32_t r = div255(sr * a + dr * inv);         // 四舍五入（与 SIMD 一致）
> ```

上面 `div255` 的展开式能直接翻译成 SIMD——
加法、移位、加法、移位，全是 `_mm_add_epi16` / `_mm_srli_epi16`，没有除法。

### 6.4 向量化思路

一次处理 4 个像素（SSE2）。难点在于：**4 个 uint32 里，颜色通道是交错打包的**。

```
内存:  [B0 G0 R0 A0] [B1 G1 R1 A1] [B2 G2 R2 A2] [B3 G3 R3 A3]
         ↑ 小端序，uint32 的 0xAARRGGBB 在内存里是 B,G,R,A
```

**第一步：拆成 16 位**

```cpp
__m128i zero = _mm_setzero_si128();
__m128i s_lo = _mm_unpacklo_epi8(s, zero);   // 前 2 个像素 → 8 个 uint16
__m128i s_hi = _mm_unpackhi_epi8(s, zero);   // 后 2 个像素
```

`unpacklo_epi8(a, zero)` 的效果是把低 8 个字节每个扩展成 16 位：

```
s      = [B0 G0 R0 A0 | B1 G1 R1 A1 | B2 G2 R2 A2 | B3 G3 R3 A3]
s_lo   = [B0 G0 R0 A0 B1 G1 R1 A1]   ← 8 个 uint16
s_hi   = [B2 G2 R2 A2 B3 G3 R3 A3]
```

现在每个通道都在独立的 16 位 lane 里了，不会互相污染。

**第二步：把 alpha 广播到同一像素的 4 个通道**

`s_lo` 是 `[B0 G0 R0 A0 | B1 G1 R1 A1]`，我需要 `[a0 a0 a0 a0 | a1 a1 a1 a1]`。

```cpp
__m128i a_lo = _mm_shufflelo_epi16(s_lo, 0xFF);  // 低 4 lane 全取第 3 个（A0）
a_lo = _mm_shufflehi_epi16(a_lo, 0xFF);          // 高 4 lane 全取第 7 个（A1）
```

`0xFF` 是立即数，含义是「新的 [`0,1,2,3`] = 旧的 `[_3,_3,_3,_3]`」。
每个 2 位段选一个源 lane：`0xFF = 11 11 11 11` → 全选 lane 3。

**第三步：乘 + 除 + 加**

```cpp
__m128i inv = _mm_sub_epi16(_mm_set1_epi16(255), a_lo);   // 255 - a
__m128i t1  = _mm_mullo_epi16(s_lo, a_lo);                // src*a
__m128i t2  = _mm_mullo_epi16(d_lo, inv);                 // dst*(255-a)
__m128i r1  = div255_epi16(t1);                           // 各自除 255
__m128i r2  = div255_epi16(t2);
__m128i out = _mm_add_epi16(r1, r2);                      // ≤ 255
```

**为什么先除再加**：`t1 + t2` 会到 130050，超出 16 位。
分开除之后再相加，每边 ≤255，所以相加 ≤510，**还是在 16 位范围里，安全**。

为什么"加起来 ≤510"其实不会真到 510：
因为 `a + inv = 255`，所以 `(sr·a + dr·inv) / 255 ≤ 255` 是恒成立的
（两个加数被 255 拉住了，不是两个独立的 255）。
分开四舍五入后最多到 256，最后 `packus` 饱和回 255，没问题。

代价：两次舍入，误差最多 1/255 —— 肉眼绝对看不出来。

### 6.5 SSE2 完整实现

```cpp
// ---------- simd_kernels.cpp ----------
#include <immintrin.h>
#include <cstdint>
#include <cstddef>

namespace simd {

inline __m128i div255_epi16(__m128i x) {
    // (x + 128 + ((x+128) >> 8)) >> 8
    const __m128i k128 = _mm_set1_epi16(128);
    __m128i t = _mm_add_epi16(x, k128);
    __m128i u = _mm_srli_epi16(t, 8);
    return _mm_srli_epi16(_mm_add_epi16(t, u), 8);
}

// 对 4 个像素做 src over dst，结果写回 dst
inline void blend4_sse2(uint32_t* dst, const uint32_t* src) {
    const __m128i zero = _mm_setzero_si128();

    __m128i s = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src));
    __m128i d = _mm_loadu_si128(reinterpret_cast<const __m128i*>(dst));

    // 拆 16 位：低 2 像素 / 高 2 像素
    __m128i s_lo = _mm_unpacklo_epi8(s, zero);
    __m128i s_hi = _mm_unpackhi_epi8(s, zero);
    __m128i d_lo = _mm_unpacklo_epi8(d, zero);
    __m128i d_hi = _mm_unpackhi_epi8(d, zero);

    // 广播 alpha
    __m128i a_lo = _mm_shufflehi_epi16(_mm_shufflelo_epi16(s_lo, 0xFF), 0xFF);
    __m128i a_hi = _mm_shufflehi_epi16(_mm_shufflelo_epi16(s_hi, 0xFF), 0xFF);

    const __m128i k255 = _mm_set1_epi16(255);
    __m128i inv_lo = _mm_sub_epi16(k255, a_lo);
    __m128i inv_hi = _mm_sub_epi16(k255, a_hi);

    // (src*a + dst*(255-a)) / 255，分两步避免溢出
    __m128i r_lo = _mm_add_epi16(
        div255_epi16(_mm_mullo_epi16(s_lo, a_lo)),
        div255_epi16(_mm_mullo_epi16(d_lo, inv_lo)));

    __m128i r_hi = _mm_add_epi16(
        div255_epi16(_mm_mullo_epi16(s_hi, a_hi)),
        div255_epi16(_mm_mullo_epi16(d_hi, inv_hi)));

    // 窄化回 8 位（packus 自带 0~255 饱和）
    __m128i out = _mm_packus_epi16(r_lo, r_hi);

    // 强制 alpha = 255（上面把 alpha 通道也算了，结果无意义）
    out = _mm_or_si128(out, _mm_set1_epi32(0xFF000000u));

    _mm_storeu_si128(reinterpret_cast<__m128i*>(dst), out);
}

// 整行混合：n = 像素个数
void blend_row_sse2(uint32_t* dst, const uint32_t* src, int n) {
    int i = 0;
    for (; i + 4 <= n; i += 4)
        blend4_sse2(dst + i, src + i);
    // 尾巴：老老实实标量
    for (; i < n; ++i) {
        extern void blend_pixel_scalar(uint32_t&, uint32_t);
        blend_pixel_scalar(dst[i], src[i]);
    }
}

} // namespace simd
```

**逐段对照检查表**（写完自己对一遍）：

| 检查项 | 应该是 |
|--------|--------|
| `n` 的含义 | 像素个数，不是字节数 |
| 循环条件 | `i + 4 <= n` |
| 尾部 | 有标量兜底 |
| 加载指令 | `loadu`（源图不保证对齐） |
| `mullo` 输入 | ≤255，乘积 ≤65025，不溢出 |
| 两次乘法是否相加后再除 | **不是**，是分别除完再加 |
| `packus` 顺序 | `packus(r_lo, r_hi)`，低半在前 |
| alpha | 最后强制 0xFF |

### 6.6 AVX2 版本 ⚠️（这一节我改过，第一版是错的）

我最初写这节时，以为「把 4 换成 8、`_mm_` 换成 `_mm256_`」就行。
**审核时用实测打脸了**：8 个像素里错了 4 个，顺序是乱的。

```
输入 8 个像素（alpha=0x80，颜色递增）
标量: FF070F17 FF070F2A FF070F3D FF070F4F FF070F62 FF070F74 FF070F87 FF071019
旧版: FF081018 FF08102B FF081062 FF081075 FF08103D FF081050 FF081087 FF08111A
                        ↑ 第 3、5 个值跳位了 —— 顺序乱了
```

**根因**：`_mm256_unpacklo_epi8` / `_mm256_packus_epi16` 在 256 位上
**是按两个 128 位半区各自独立工作的**，不是把 256 位当一整块。

```
输入 s = [p0 p1 p2 p3 | p4 p5 p6 p7]     (| 是 128 位半区分界)

_mm256_unpacklo_epi8(s, 0) 得到的是：
    [p0 p1 | p4 p5]        ← 不是 [p0 p1 p2 p3]！

_mm256_unpackhi_epi8(s, 0) 得到的是：
    [p2 p3 | p6 p7]
```

每个半区内部各自 interleave，跨半区不会混。
于是 `packus(r1, r2)` 之后再怎么 `permute4x64` 也拼不回原顺序——
**因为乱的不是 64 位块的顺序，是像素本身在半区里的分布。**

#### 正确做法：拆成两个 128 位半区，各跑一遍 SSE2 内核

```cpp
// 一次处理 8 个像素：拆成两个 128 位半区
static void blend8_avx2(uint32_t* dst, const uint32_t* src) {
    // 低 4 像素
    __m128i s_lo = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + 0));
    __m128i d_lo = _mm_loadu_si128(reinterpret_cast<const __m128i*>(dst + 0));
    // 高 4 像素
    __m128i s_hi = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + 4));
    __m128i d_hi = _mm_loadu_si128(reinterpret_cast<const __m128i*>(dst + 4));

    _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + 0), blend4_core(s_lo, d_lo));
    _mm_storeu_si128(reinterpret_cast<__m128i*>(dst + 4), blend4_core(s_hi, d_hi));
}
```

其中 `blend4_core` 就是把 6.5 节 `blend4_sse2` 里的运算部分抽出来：

```cpp
static inline __m128i blend4_core(__m128i s, __m128i d) {
    const __m128i zero = _mm_setzero_si128();
    __m128i s_lo = _mm_unpacklo_epi8(s, zero);
    __m128i s_hi = _mm_unpackhi_epi8(s, zero);
    __m128i d_lo = _mm_unpacklo_epi8(d, zero);
    __m128i d_hi = _mm_unpackhi_epi8(d, zero);

    // 把每个像素的 alpha 广播到它自己的 4 个通道
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
    return _mm_or_si128(out, _mm_set1_epi32(0xFF000000u));   // 强制 alpha=255
}
```

**这个版本不需要 `permute4x64`** —— 因为压根没做跨半区操作。

#### 那 `permute4x64` 什么时候才需要？

只有当你**坚持用 256 位指令**做 unpack/pack 时才需要，而且光靠它不够，
还得在 unpack **之前**先重排一次输入。绕，收益还不大。

**结论**：
- 简单正确 → 拆成两个 128 位半区（本节方案）✅
- 极限性能 → 用 256 位 mullo + 手动处理半区，先想清楚再写

在支持 AVX2 的 CPU 上，SSE2 指令照样能跑满，拆半区的损失远小于
「调三个小时顺序 bug」。**先让 SIMD 版正确，再考虑 AVX2 的最大化**。

#### 一个必须说清楚的事：这个文件其实没用 AVX2 指令

我把上面的代码拿去编译时发现：**不开 `/arch:AVX2` 也能编译通过**。

因为修正版里全是 `_mm_loadu_si128` / `_mm_mullo_epi16` 这些 **SSE2（128 位）指令**，
只是"一次处理 8 个像素"而已，并没有用到 256 位的 `ymm` 寄存器。

所以：

| | 说明 |
|---|---|
| 文件名 `KernelsAVX2.cpp` | **有误导性**，它只是"SSE2 内核 ×2" |
| 要不要开 `/arch:AVX2` | 这个版本**不需要**；开了也没坏处 |
| 那 AVX2 的意义在哪 | 在于 CPU 支持 AVX2 时可以安全启用更宽的指令；本项目留作进阶 |

**如果你想要真正的 256 位加速**，思路是这样（**注意：下面只是示意，不能直接用**）：

```cpp
// 真·AVX2 的乘法部分 —— 但 unpack/pack 的半区问题要自己解决
__m256i v = _mm256_mullo_epi16(s16, a16);         // 16 个 uint16 一次乘完
// 你需要自己写一个 256 位版的 div255：
// (x + 128 + ((x+128) >> 8)) >> 8
__m256i t = _mm256_add_epi16(v, _mm256_set1_epi16(128));
__m256i u = _mm256_srli_epi16(t, 8);
__m256i r = _mm256_srli_epi16(_mm256_add_epi16(t, u), 8);
```

**这段乘法是安全的**（`_mm256_mullo_epi16` 逐 lane 独立，没有半区问题）。
真正麻烦的是它前后的 `unpack` / `packus` —— 那两步按 128 位半区独立工作，
必须自己做重排。**先写好黄金测试再改，别跳步。**

我实测过：直接按"8 个像素"的直觉写，会错 4/8 个像素。

**给初学者的建议**：先跑通本节这个"128 位 ×2"的版本，测个速。
如果它已经达到你的帧率目标，**就别去碰 256 位了**。
`blend` 是内存带宽受限的，位宽翻倍带来的收益通常不到 30%，
不值得为此付出调试成本。

> 顺带说一句：如果想让画面真的更快，别在 `blend` 上较劲，
> 去优化那个**每个像素都要跑一遍**的全屏 tint（第 7 节）。

### 6.7 正确性验证：黄金测试

**不要靠"看起来对"**。写一个测试，拿标量结果当黄金标准：

```cpp
#include <cassert>
#include <random>

void test_blend() {
    std::mt19937 rng(12345);
    std::uniform_int_distribution<uint32_t> dist(0, 0xFFFFFFFF);

    for (int iter = 0; iter < 10000; ++iter) {
        uint32_t src[64], d0[64], d1[64];
        for (int i = 0; i < 64; ++i) {
            src[i] = dist(rng);
            d0[i] = d1[i] = dist(rng);
        }

        // 标量
        for (int i = 0; i < 64; ++i) blend_pixel_scalar(d0[i], src[i]);
        // SIMD
        blend_row_sse2(d1, src, 64);

        for (int i = 0; i < 64; ++i) {
            int dr = int((d0[i] >> 16) & 0xFF) - int((d1[i] >> 16) & 0xFF);
            int dg = int((d0[i] >> 8)  & 0xFF) - int((d1[i] >> 8)  & 0xFF);
            int db = int( d0[i]        & 0xFF) - int( d1[i]        & 0xFF);
            // 允许 1 的误差（两次舍入）
            assert(std::abs(dr) <= 1 && std::abs(dg) <= 1 && std::abs(db) <= 1);
        }
    }
    printf("blend 一致性测试通过\n");
}
```

**误差容忍 1 是正常的**（6.4 节说过原因）。
如果误差是 5 以上，说明哪一步拆错了；如果是几百，通常是 `packus` 顺序反了。

### 6.8 附：`broadcast_alpha_256`（备用，当前方案用不上）

如果你在 6.6 里选择了"坚持用 256 位指令"的进阶路线，
会需要把 alpha 广播到 256 位寄存器。AVX2 没有 `_mm256_shufflelo_epi16`，
只能拆成两个 128 位半区分别 shuffle 再拼回来：

```cpp
// 输入：8 个 uint16（4 个像素的 B,G,R,A）
// 输出：每个像素的 4 个通道都填成它自己的 alpha
inline __m256i broadcast_alpha_256(__m256i v) {
    __m128i lo = _mm256_castsi256_si128(v);
    __m128i hi = _mm256_extracti128_si256(v, 1);
    lo = _mm_shufflehi_epi16(_mm_shufflelo_epi16(lo, 0xFF), 0xFF);
    hi = _mm_shufflehi_epi16(_mm_shufflelo_epi16(hi, 0xFF), 0xFF);
    return _mm256_set_m128i(hi, lo);
}
```

> 注意：6.6 的最终方案（拆两个 128 位半区跑 SSE2 内核）**用不到这个函数**。
> 它只在"真·AVX2 256 位写法"那条进阶路线里才有用。先别管，真要用的时候再回来看。

---

## 7. 颜色调制：天气和受击闪白

这个是**性价比最高**的 SIMD 应用，因为公式简单，而且每帧都要用。

### 7.1 需求

- 雨天：整屏压暗 + 偏蓝
- 雪天：偏白 + 略亮
- 地窖：整体偏暖（火光）
- 敌人受击：整个精灵闪白一帧

统一抽象成：

```
out = src × tint / 255
```

`tint = 0xFFRRGGBB`，每个通道独立乘。

### 7.2 标量版

```cpp
inline uint32_t tint_pixel(uint32_t s, uint32_t t) {
    uint32_t r = ((s >> 16) & 0xFF) * ((t >> 16) & 0xFF) / 255;
    uint32_t g = ((s >> 8)  & 0xFF) * ((t >> 8)  & 0xFF) / 255;
    uint32_t b = ( s        & 0xFF) * ( t        & 0xFF) / 255;
    return (s & 0xFF000000u) | (r << 16) | (g << 8) | b;
}
```

### 7.3 SIMD 版

结构跟 blending 一样：拆 16 位 → 乘 → 除 255 → pack。

**但没有溢出问题**：`255 × 255 = 65025`，单个乘法安全，不需要拆两步。

```cpp
inline __m128i tint4_sse2(__m128i s, __m128i t) {
    const __m128i zero = _mm_setzero_si128();
    __m128i s_lo = _mm_unpacklo_epi8(s, zero);
    __m128i s_hi = _mm_unpackhi_epi8(s, zero);
    __m128i t_lo = _mm_unpacklo_epi8(t, zero);   // t 是广播后的
    __m128i t_hi = _mm_unpackhi_epi8(t, zero);

    __m128i r_lo = div255_epi16(_mm_mullo_epi16(s_lo, t_lo));
    __m128i r_hi = div255_epi16(_mm_mullo_epi16(s_hi, t_hi));
    return _mm_packus_epi16(r_lo, r_hi);
}
```

**顺序很重要**：先 tint 再 blend。
因为 alpha 通道不参与 tint（tint 的 alpha 是 255，乘完 alpha 不变），
如果先 blend 再 tint，半透明边缘会被错误调制两次。

### 7.4 「整屏压暗」的最快做法

如果只是压暗（所有通道乘同一个系数），根本不用拆 16 位：

```cpp
// 每个通道右移 = 除以 2^n，配合 mask 防止跨通道污染
__m128i s = _mm_loadu_si128(...);
__m128i masked = _mm_and_si128(s, _mm_set1_epi32(0x00FEFEFEu));  // 保留低位防借位
__m128i dark = _mm_srli_epi32(masked, 1);                        // 亮度减半
```

**这个技巧的坑**：直接 `_mm_srli_epi32(s, 1)` 会让低位通道（B）右移时把高位通道的低位挤进来。
先用 `0x00FEFEFE` mask 掉每个字节的最低位，再整体右移，就不会串。

**速度**：比乘法版快 3 倍以上。天气压暗每帧都要做，值得优化。

---

## 8. 运行时派发

### 8.1 CPUID 探测（MSVC）

```cpp
#include <intrin.h>
#include <cstring>

namespace simd {

struct CpuInfo { bool sse2 = false, sse41 = false, avx2 = false; };

inline CpuInfo probe_cpu() {
    CpuInfo info;
    int regs[4];

    __cpuid(regs, 0);
    const int max_id = regs[0];
    if (max_id < 1) return info;

    __cpuid(regs, 1);
    info.sse2  = (regs[3] & (1 << 26)) != 0;
    info.sse41 = (regs[2] & (1 << 19)) != 0;

    // AVX2 在 leaf 7 的 EBX bit 5，且需要 OS 支持 XSAVE
    if (max_id >= 7) {
        __cpuidex(regs, 7, 0);
        info.avx2 = (regs[1] & (1 << 5)) != 0;

        // 还要检查 OS 是否启用了 YMM 状态保存
        unsigned long long xcr0 = _xgetbv(0);
        if ((xcr0 & 0x6) != 0x6) info.avx2 = false;
    }
    return info;
}

Isa detect() {
    static const Isa isa = [] {
        CpuInfo c = probe_cpu();
        if (c.avx2)  return Isa::AVX2;
        if (c.sse2)  return Isa::SSE2;
        return Isa::Scalar;
    }();
    return isa;
}

Isa active() { return detect(); }

} // namespace simd
```

`static const` 局部变量保证只探测一次（C++11 起的线程安全初始化）。

### 8.2 派发方式

**方式 A：`switch`（本项目用这个）**

```cpp
inline void blend_row(Pixel* d, const Pixel* s, int n) {
    switch (active()) {
        case Isa::AVX2: return blend_row_avx2(d, s, n);
        case Isa::SSE2: return blend_row_sse2(d, s, n);
        default:        return blend_row_scalar(d, s, n);
    }
}
```

缺点：每行都判断一次。但 `blend_row` 一次处理上千像素，这点开销可以忽略。

**方式 B：函数指针表（每帧只查一次）**

```cpp
struct Backend {
    void (*blend_row)(Pixel*, const Pixel*, int);
    void (*tint_row)(Pixel*, const Pixel*, int);
};

const Backend& backend() {
    static const Backend avx2   { blend_row_avx2,   tint_row_avx2   };
    static const Backend sse2   { blend_row_sse2,   tint_row_sse2   };
    static const Backend scalar { blend_row_scalar, tint_row_scalar };

    static const Backend* chosen = [] {
        switch (detect()) {
            case Isa::AVX2: return &avx2;
            case Isa::SSE2: return &sse2;
            default:        return &scalar;
        }
    }();
    return *chosen;
}
```

用 `backend().blend_row(...)` 调用。**推荐这个**，尤其是内层循环里要反复调的时候。

### 8.3 加日志，别猜

启动时打一行：

```cpp
const char* isa_name(Isa i) {
    switch (i) {
        case Isa::AVX2: return "AVX2";
        case Isa::SSE2: return "SSE2";
        default:        return "scalar";
    }
}
printf("[simd] 后端: %s\n", isa_name(simd::active()));
```

**没有这行日志，你永远不知道自己测的是哪条路径。**
我见过太多人"优化了半天"，结果跑的是标量分支。

---

## 9. 错误清单（按踩坑频率排序）

| 症状 | 原因 | 修法 |
|------|------|------|
| 一运行就崩，崩在 `movdqa` | 用了对齐 load，地址非 16 倍数 | 换 `loadu`，或修 `pitch` 对齐 |
| 画面出现左右错位条纹 | 用了 256 位 unpack/pack，像素在半区里分布乱了 | 用 6.6 的拆半区方案；或者 unpack 前先重排 |
| 颜色发绿/发紫 | 通道顺序搞错（BGR vs RGB） | 记住内存是 B,G,R,A；`0xAARRGGBB` |
| 边缘出现黑边 | alpha 通道被当成颜色算了 | 最后 `or` 上 `0xFF000000` |
| 最后几个像素是花的 | 忘了尾部标量循环 | `for (; i < n; ++i)` |
| 偶发崩溃（只有某些宽度） | `pitch` 没按 8 对齐 | `pitch = (w + 7) & ~7` |
| 结果整体偏暗 | 溢出导致回绕 | 检查乘法是否 ≤65535，加法是否拆开 |
| 在老机器上崩 | 全局开了 `/arch:AVX2` | 只给 avx2.cpp 开，运行时检测 |
| 测速没提升 | 跑的是标量分支 / 被编译器优化掉了 | 加日志；用 `volatile` 或真实数据 |

---

## 10. 进阶：预乘 alpha（可选，快 30%）

如果用**预乘 alpha**存图（`rgb *= a/255` 在加载时做一次），混合公式变成：

```
out = src + dst × (255 − a) / 255
```

好处：
- 少一次乘法
- 少一次除法
- `src + ...` 可以用饱和加 `_mm_adds_epu8`，一条指令搞定

坏处：
- 图集要预处理
- 从图集里"取原始颜色"（比如做拾色器）时要反算
- 半透明区域叠多次会累积误差

**什么时候值得**：绘制调用里 90% 都是半透明叠加时。
**什么时候不值得**：你的场景大部分是不透明精灵（本项目就是）——那不透明的快速路径收益更大。

> 本项目的路线：**先做非预乘**（第 6 节），跑通、测速。
> 如果发现 `blit_alpha` 真的是瓶颈，再改预乘。
> **优化要有测量支撑，不要有"我觉得这里慢"。**

---

## 11. 本章自检

做完这些题，你才算真会了：

1. 把 `blend4_sse2` 的 `_mm_shufflelo_epi16(s_lo, 0xFF)` 改成 `0xAA`，
   猜结果会变成什么，然后跑黄金测试验证你的猜想。
2. 不查资料，写出 `div255_epi16`，并证明它在 `x ≤ 65025` 时精确。
3. 为什么 `_mm_mullo_epi16` 处理 255×255 不会溢出，但两个乘积相加会？
   用二进制说明。
4. `pitch = (w + 7) & ~7` 里，`& ~7` 为什么等价于"清掉低 3 位"？如果 `w = 65` 结果是多少？
5. 你的 CPU 上，`blend_row_sse2` 比 `blend_row_scalar` 快多少倍？
   如果不到 2 倍，找出原因（提示：是不是快速路径 `a==255` 让标量版太快了？）
6. 把 AVX2 版本的 `permute4x64` 去掉，跑一次，描述你看到的画面。

第 5 题的答案会很有意思：**在大量不透明像素的场景里，标量版可能并不慢**。
这就是为什么第 4 章要教你怎么测量，而不是教你怎么"优化"。
