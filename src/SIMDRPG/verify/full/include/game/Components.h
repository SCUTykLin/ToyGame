#pragma once
#include "game/Animator.h"
#include "gfx/Surface.h"

#include <cstdint>

namespace game {

// 每个组件都必须有构造函数。
//
// 原因：Registry::add 里写的是 T{args...}。
// MSVC 对"带默认成员初始化器的聚合类型"做花括号初始化有 bug（C2440），
// 显式写构造函数就绕开了。
struct Transform {
    float x = 0.f, y = 0.f;
    Transform() = default;
    Transform(float x_, float y_) : x(x_), y(y_) {}
};

struct Velocity {
    float x = 0.f, y = 0.f;
    Velocity() = default;
    Velocity(float x_, float y_) : x(x_), y(y_) {}
};

struct Health {
    int hp = 100;
    int max_hp = 100;
    float flash = 0.f;       // 受击闪白剩余时间
    Health() = default;
    Health(int h, int m) : hp(h), max_hp(m) {}
};

struct Sprite {
    const gfx::Surface* sheet = nullptr;
    Animator anim;
    int draw_w = 48;         // 画到多大（素材原始尺寸五花八门）
    int draw_h = 48;
    Sprite() = default;
    Sprite(const gfx::Surface* s, int dw, int dh)
        : sheet(s), draw_w(dw), draw_h(dh) {}
};

struct AI {
    uint32_t behavior = 0;
    float speed = 50.f;
    int   sight = 200;
    int   ranged_range = 0;      // 远程怪的开火距离
    float attack_cd = 0.f;
    float blink_cd = 0.f;
    float stun = 0.f;
    AI() = default;
    AI(uint32_t b, float s, int si, int rr = 0)
        : behavior(b), speed(s), sight(si), ranged_range(rr) {}
};

struct Flying {};                    // 标记组件，无数据

struct Pickup {
    int item_index = 0;
    int kind = 0;                    // 0=金币 1=药水 2=弹药
    Pickup() = default;
    Pickup(int i, int k) : item_index(i), kind(k) {}
};

struct Fx {
    int frame = 0;
    float life = 0.35f;
    Fx() = default;
    Fx(int f, float l) : frame(f), life(l) {}
};

struct Boss {
    int phase = 1;
    float phase_timer = 0.f;
    Boss() = default;
    explicit Boss(int p) : phase(p) {}
};

// "我是谁" —— 死亡处理需要 split_into / summon_into / is_boss，
// 这些是身份信息，不是数值，所以单独放一个组件。
struct EnemySpec;            // 前向声明就够（这里只存指针）

struct EnemyTag {
    const EnemySpec* spec = nullptr;
    EnemyTag() = default;
    explicit EnemyTag(const EnemySpec* s) : spec(s) {}
};

} // namespace game
