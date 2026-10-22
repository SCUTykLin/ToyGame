#pragma once
#include "game/Animator.h"
#include "game/Combat.h"

namespace game {

// 玩家不用 ECS。
//
// 理由：全局只有一个，而且几乎所有系统都要访问它（相机、UI、门禁、Boss）。
// 塞进 ECS 之后每处都得写 reg.get<PlayerTag>()，代码更长、更慢、更容易错。
// ECS 适合"数量多、结构一致"的东西，玩家不是。
struct Player {
    float x = 0.f, y = 0.f;
    float speed = 120.f;

    int move_dir_x = 0, move_dir_y = 0;
    bool want_attack   = false;
    bool want_interact = false;

    int   hp = 100;
    int   max_hp = 100;
    float invuln = 0.f;          // 无敌剩余时间
    float attack_cd = 0.f;

    AttackState attack;
    Animator    anim;
    bool        moving = false;

    bool can_be_hit() const { return invuln <= 0.f && hp > 0; }

    void take_damage(int dmg) {
        if (!can_be_hit()) return;
        hp -= dmg;
        invuln = 0.9f;           // 受击后 0.9 秒无敌，防止被围住秒杀
    }

    void update(float dt) {
        if (invuln > 0.f)    invuln -= dt;
        if (attack_cd > 0.f) attack_cd -= dt;

        attack.update(dt);

        moving = (move_dir_x != 0 || move_dir_y != 0);
        anim.update(dt, moving, move_dir_x, move_dir_y);
    }
};

} // namespace game
