#pragma once
#include "game/Combat.h"
#include "gfx/Surface.h"
#include "ecs/Registry.h"     // 需要 ecs::Entity 的完整定义

namespace game {

namespace B {
    constexpr uint32_t Chase  = 1u << 0;
    constexpr uint32_t Ranged = 1u << 1;
    constexpr uint32_t Flying = 1u << 2;
    constexpr uint32_t Split  = 1u << 3;
    constexpr uint32_t Summon = 1u << 4;
    constexpr uint32_t Charge = 1u << 5;
    constexpr uint32_t Blink  = 1u << 6;
    constexpr uint32_t Static = 1u << 7;
    constexpr uint32_t Poison = 1u << 8;
    constexpr uint32_t Shield = 1u << 9;
}

struct EnemySpec {
    const char* name;
    const char* sprite;      // 相对 assets/gfx/

    int hp, atk, speed;

    // 素材规格（每个都不一样，见 03b 7.3）
    int fw, fh;              // 单帧尺寸
    int frames;              // 每行几帧
    int rows;                // 图集几行（LPC 角色 4，怪物 1）

    // 画到多大（统一视觉尺寸）
    int draw_w, draw_h;

    uint32_t behavior;
    int sight;

    int ranged_range = 0;
    const char* split_into  = nullptr;
    const char* summon_into = nullptr;
    bool is_boss = false;
};

extern const EnemySpec ENEMIES[];
extern const int       ENEMY_COUNT;

const EnemySpec* find_enemy(const char* name);

// 带缓存的贴图加载：同一张图只读一次。
// NPC 也用这个（它们不走 ENEMIES[] 表）。
const gfx::Surface* load_sheet(const char* rel);

ecs::Entity spawn_enemy(ecs::Registry& reg, const char* name, float x, float y);

// ---- 实体生成工具（战斗/掉落/特效共用） ----
ecs::Entity spawn_projectile(ecs::Registry& reg, float x, float y,
                             float vx, float vy, int dmg,
                             bool from_player, bool pierce);

ecs::Entity spawn_fx(ecs::Registry& reg, float x, float y, int pool_size);

ecs::Entity drop_item(ecs::Registry& reg, float x, float y, int pool_size);

} // namespace game
