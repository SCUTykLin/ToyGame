#include "world/Weather.h"

#include <cstdlib>
#include <cstring>
#include <immintrin.h>

namespace world {

WeatherKind from_string(const char* s) {
    if (!s) return WeatherKind::None;
    if (std::strcmp(s, "rain")   == 0) return WeatherKind::Rain;
    if (std::strcmp(s, "snow")   == 0) return WeatherKind::Snow;
    if (std::strcmp(s, "fog")    == 0) return WeatherKind::Fog;
    if (std::strcmp(s, "meteor") == 0) return WeatherKind::Meteor;
    return WeatherKind::None;          // "clear" / "none" / 未知
}

static inline int clamp_count(int n) {
    return n < 0 ? 0 : (n > ParticleSystem::MAX ? ParticleSystem::MAX : n);
}

void reset_rain(ParticleSystem& p, int count, int w, int h) {
    p.count = clamp_count(count);
    for (int i = 0; i < p.count; ++i) {
        p.x[i]  = float(std::rand() % (w > 0 ? w : 1));
        p.y[i]  = float(std::rand() % (h > 0 ? h : 1));
        p.vx[i] = 0.f;
        p.vy[i] = 400.f + float(std::rand() % 200);
    }
}

void reset_snow(ParticleSystem& p, int count, int w, int h) {
    p.count = clamp_count(count);
    for (int i = 0; i < p.count; ++i) {
        p.x[i]  = float(std::rand() % (w > 0 ? w : 1));
        p.y[i]  = float(std::rand() % (h > 0 ? h : 1));
        p.vx[i] = -30.f + float(std::rand() % 60);
        p.vy[i] = 40.f + float(std::rand() % 60);
    }
}

void reset_meteor(ParticleSystem& p, int count, int w, int h) {
    p.count = clamp_count(count);
    for (int i = 0; i < p.count; ++i) {
        p.x[i]  = float(std::rand() % (w > 0 ? w * 2 : 1)) - float(w) * 0.5f;
        p.y[i]  = -float(std::rand() % (h > 0 ? h : 1));
        p.vx[i] = 300.f;
        p.vy[i] = 700.f;
    }
}

// ---------------------------------------------------------------
// 标量版：黄金标准
// ---------------------------------------------------------------
void update_particles_scalar(ParticleSystem& p, float dt, int screen_h) {
    const int w = 640;          // 横向回绕边界，和屏幕等宽
    for (int i = 0; i < p.count; ++i) {
        p.x[i] += p.vx[i] * dt;
        p.y[i] += p.vy[i] * dt;

        if (p.y[i] > float(screen_h)) {
            p.y[i] = 0.f;
            p.x[i] = float(std::rand() % w);
        }
        // 横向回绕：出左边从右边进来
        if (p.x[i] < 0.f)    p.x[i] += float(w);
        if (p.x[i] > float(w)) p.x[i] -= float(w);
    }
}

// ---------------------------------------------------------------
// SIMD 版：一次 8 个
// ---------------------------------------------------------------
void update_particles_simd(ParticleSystem& p, float dt, int screen_h) {
    const __m256 dtv = _mm256_set1_ps(dt);
    const __m256 hh  = _mm256_set1_ps(float(screen_h));
    const __m256 zero = _mm256_setzero_ps();
    const __m256 wf  = _mm256_set1_ps(640.f);

    int i = 0;
    for (; i + 8 <= p.count; i += 8) {
        __m256 x  = _mm256_load_ps(p.x  + i);     // 对齐加载，数组是 alignas(32)
        __m256 y  = _mm256_load_ps(p.y  + i);
        const __m256 vx = _mm256_load_ps(p.vx + i);
        const __m256 vy = _mm256_load_ps(p.vy + i);

        x = _mm256_add_ps(x, _mm256_mul_ps(vx, dtv));
        y = _mm256_add_ps(y, _mm256_mul_ps(vy, dtv));

        // y > screen_h → 回到顶部
        const __m256 over = _mm256_cmp_ps(y, hh, _CMP_GT_OQ);
        y = _mm256_blendv_ps(y, zero, over);      // 命中的通道取 0

        // 横向回绕用减法/加法各一次，不需要随机（随机数没法向量化）
        const __m256 lt0 = _mm256_cmp_ps(x, zero, _CMP_LT_OQ);
        x = _mm256_add_ps(x, _mm256_and_ps(lt0, wf));       // x<0 时 +640

        const __m256 gt = _mm256_cmp_ps(x, wf, _CMP_GT_OQ);
        x = _mm256_sub_ps(x, _mm256_and_ps(gt, wf));        // x>640 时 -640

        _mm256_store_ps(p.x + i, x);
        _mm256_store_ps(p.y + i, y);
    }

    // 尾部
    for (; i < p.count; ++i) {
        p.x[i] += p.vx[i] * dt;
        p.y[i] += p.vy[i] * dt;
        if (p.y[i] > float(screen_h)) p.y[i] = 0.f;
        if (p.x[i] < 0.f)       p.x[i] += 640.f;
        if (p.x[i] > 640.f)     p.x[i] -= 640.f;
    }
}

} // namespace world
