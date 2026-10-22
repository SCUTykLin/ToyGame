#pragma once
#include "core/Types.h"

namespace game {

// 一次挥击的状态。跟动画无关，纯计时。
struct AttackState {
    bool  active   = false;
    float timer    = 0.f;
    float duration = 0.12f;
    int   dir      = 2;          // 0上 1左 2下 3右
    bool  hit_done = false;      // 这次挥击已经打中过了

    void trigger(int d) {
        active   = true;
        timer    = 0.f;
        dir      = d;
        hit_done = false;        // 关键：每次挥击重置
    }

    void update(float dt) {
        if (!active) return;
        timer += dt;
        if (timer >= duration) active = false;
    }
};

// 角色左上角在 (px,py)，朝 dir，攻击距离 range
inline core::Rect melee_box(float px, float py, int dir, float range) {
    const int r  = int(range);
    const int cx = int(px) + 32;
    const int cy = int(py) + 32;

    switch (dir) {
        case 0:  return { cx - 16, cy - r,  32, r  };   // 上
        case 1:  return { cx - r,  cy - 16, r,  32 };   // 左
        case 2:  return { cx - 16, cy + 16, 32, r  };   // 下
        default: return { cx + 16, cy - 16, r,  32 };   // 右
    }
}

struct Weapon {
    // 内嵌枚举：这样调用处写 Weapon::Kind::Melee，读起来才知道是"武器的种类"
    enum class Kind { Melee, Ranged, Laser };

    const char* name;
    Kind        kind;
    int         damage;
    float       range;        // 近战：判定框长度；远程：子弹存活距离
    float       cooldown;
    int         ammo;         // -1 = 无限
    bool        pierce;       // 穿透
};

extern const Weapon WEAPONS[];
extern const int    WEAPON_COUNT;

// 投射物组件
struct Projectile {
    int   damage      = 10;
    float life        = 1.8f;
    bool  pierce      = false;
    bool  from_player = true;

    Projectile() = default;
    Projectile(int d, float l, bool p, bool fp)
        : damage(d), life(l), pierce(p), from_player(fp) {}
};

} // namespace game
