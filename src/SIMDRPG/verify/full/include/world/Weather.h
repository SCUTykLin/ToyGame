#pragma once

namespace world {

// SoA 布局：每个字段一个数组。
// alignas(32) 是给 _mm256_load_ps 用的（它要求 32 字节对齐）。
struct ParticleSystem {
    static constexpr int MAX = 4096;

    alignas(32) float x[MAX]  = {};
    alignas(32) float y[MAX]  = {};
    alignas(32) float vx[MAX] = {};
    alignas(32) float vy[MAX] = {};

    int count = 0;
};

enum class WeatherKind { None, Rain, Snow, Fog, Meteor };

WeatherKind from_string(const char* s);

void reset_rain  (ParticleSystem& p, int count, int w, int h);
void reset_snow  (ParticleSystem& p, int count, int w, int h);
void reset_meteor(ParticleSystem& p, int count, int w, int h);

void update_particles_scalar(ParticleSystem& p, float dt, int screen_h);
void update_particles_simd  (ParticleSystem& p, float dt, int screen_h);

} // namespace world
