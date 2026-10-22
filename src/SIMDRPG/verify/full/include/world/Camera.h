#pragma once
#include <algorithm>
#include <cstdlib>

namespace world {

struct Camera {
    int x = 0, y = 0;              // 屏幕左上角对应的世界坐标

    // 震屏
    float shake_time = 0.f;
    float shake_mag  = 0.f;

    void follow(int target_x, int target_y, int view_w, int view_h,
                int map_w_px, int map_h_px) {
        x = target_x - view_w / 2;
        y = target_y - view_h / 2;

        const int max_x = std::max(0, map_w_px - view_w);
        const int max_y = std::max(0, map_h_px - view_h);
        x = std::clamp(x, 0, max_x);
        y = std::clamp(y, 0, max_y);
    }

    void shake(float mag, float duration) {
        shake_mag  = mag;
        shake_time = duration;
    }

    void update(float dt) {
        if (shake_time <= 0.f) return;
        shake_time -= dt;
        if (shake_time <= 0.f) {
            shake_time = 0.f;
            shake_mag  = 0.f;
        }
    }

    // 绘制时用这两个，不要直接用 x / y
    int render_x() const { return shake_time > 0.f ? x + offset() : x; }
    int render_y() const { return shake_time > 0.f ? y + offset() : y; }

private:
    // 最后 0.1 秒逐渐减弱，不然震屏会"啪"地停住，很生硬
    int offset() const {
        const float k = shake_time > 0.1f ? 1.f : shake_time * 10.f;
        const int amp = int(shake_mag * k);
        if (amp <= 0) return 0;
        return (std::rand() % (amp * 2 + 1)) - amp;
    }
};

} // namespace world
