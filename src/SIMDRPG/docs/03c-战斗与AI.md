# 03c · 战斗与 AI（阶段 10–11）

> **这一册的目标**：让"能走的世界"变成"能打的世界"。
> 结束时你有近战判定、投射物、5 把武器、25 种普通怪 + 3 个 Boss，
> 全部由一张配置表驱动。

---

## 阶段 10 · 战斗

### 10.1 先做一个重要的简化

真正的动作游戏有"攻击动画帧 → 攻击判定框"的对齐问题：
第 3 帧抬手、第 5 帧刀锋到达最远、第 7 帧收招。
这需要逐帧标定判定框，工作量很大，而且素材（LPC 战斗动画）本身是残缺的。

本项目的做法：**攻击是一个短时间的矩形，跟动画无关**。

```
按下攻击 → 立刻产生一个持续 0.12 秒的矩形
          → 这 0.12 秒内碰到矩形的东西受伤
          → 每个目标只受伤一次（hit_done）
```

好处：判定稳定、可测试、不依赖动画帧。
坏处：手感偏"钝"，没有刀锋扫过的精确感。
对一个教学项目，这是正确的取舍。

### 10.2 攻击判定的三种形态

| 形态 | 判定方式 | 用在哪 |
|------|----------|--------|
| 近战矩形 | 角色前方一个矩形 | 剑、棍 |
| 投射物 | 一个小实体带速度，飞行中检测 | 枪、激光 |
| 接触伤害 | 敌人碰撞盒和玩家碰撞盒重叠 | 所有敌人 |

### 10.3 近战：前方矩形

角色精灵 64×64，中心在 `(px+32, py+32)`。攻击框从中心往一个方向伸出 `range`：

```
dir=0（上）:  (cx-16, cy-range, 32, range)
dir=1（左）:  (cx-range, cy-16, range, 32)
dir=2（下）:  (cx-16, cy+16, 32, range)
dir=3（右）:  (cx+16, cy-16, range, 32)
```

**为什么从中心算，不从左上角算**：
从左上角算的话，四个方向都要加 `+32` 的偏移，四行代码各不同，容易写错一个。
从中心算，四个方向对称，一眼能看出对不对。

### 10.4 要建的文件

```
include/game/Combat.h
include/game/Player.h
include/game/Inventory.h
```

### 10.5 `include/game/Combat.h`

<!-- FILE: include/game/Combat.h -->
```cpp
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
```

### 10.6 `include/game/Player.h`

<!-- FILE: include/game/Player.h -->
```cpp
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
```

**`invuln` 为什么只有玩家有**：
敌人不需要"受击无敌"——多段伤害打在同一只怪上是合理的。
玩家需要，因为 3 只怪同时贴上来时，每帧都扣血会在 0.2 秒内秒杀，
玩家连反应的机会都没有。0.9 秒是一个经验值：够你走开，又不会太安全。

### 10.7 `include/game/Inventory.h`

<!-- FILE: include/game/Inventory.h -->
```cpp
#pragma once

namespace game {

struct Inventory {
    int weapon_index = 0;
    int ammo   = 0;      // 只对远程武器有意义
    int gold   = 0;
    int potions = 0;

    void next_weapon(int count) {
        if (count <= 0) return;
        weapon_index = (weapon_index + 1) % count;
    }
};

} // namespace game
```

### 10.8 命中判定：为什么需要 `hit_done`

```cpp
if (player_.attack.active && !player_.attack.hit_done) {
    const Weapon& w = WEAPONS[inv_.weapon_index];
    if (w.kind == Weapon::Kind::Melee) {
        const core::Rect box = melee_box(player_.x, player_.y,
                                         player_.attack.dir, w.range);
        bool hit_any = false;
        reg_.each<Transform, Health>([&](ecs::Entity, Transform& t, Health& h) {
            if (h.hp <= 0) return;
            const core::Rect body{ int(t.x) + 16, int(t.y) + 40, 32, 20 };
            if (!core::rect_overlap(box, body)) return;
            h.hp -= w.damage;
            h.flash = 0.1f;
            hit_any = true;
        });
        if (hit_any) player_.attack.hit_done = true;
    }
}
```

**没有 `hit_done` 会怎样**：
攻击持续 0.12 秒，60 FPS 下是 7 帧。每帧都检测，同一只怪会被打 7 次，
伤害直接 ×7。

**为什么是"打中任意一个就置位"而不是"每个目标单独记"**：
一次挥击只打一个目标，是这类游戏的标准做法。
要支持"一刀多杀"，把 `hit_done` 换成 `std::vector<Entity> hit_list`，
每次检查是否已在表里。

### 10.9 投射物

投射物是**实体**，有三个组件：`Transform` + `Velocity` + `Projectile`。

更新逻辑（每帧）：

```
pos += vel * dt
life -= dt
if (life <= 0) destroy
if (出界) destroy
if (和敌人/玩家重叠) { 扣血; if (!pierce) destroy; }
```

**为什么要 `life`**：
玩家朝地图外开枪，子弹会一直飞，实体永远不销毁。
一小时之后你有几万个实体，`each` 每帧遍历几万次，帧率归零。

`life = 1.8 秒`、子弹速度 420 px/s，射程约 756 像素——比屏幕宽，够用。

### 10.10 武器的完整流程

```
按下攻击
  → attack_cd <= 0 ?
      → yes: attack_cd = w.cooldown; attack.trigger(anim.row)
             → 如果是远程: 扣弹药，生成投射物
             → 如果是近战: 什么都不做（下一帧开始判定）
      → no : 忽略（冷却中）
```

**`attack.trigger(anim.row)`**：用当前朝向作为攻击方向。
`anim.row` 就是 0/1/2/3，和 `melee_box` 的 `dir` 是同一套编号。

### 10.11 受击闪白

用 `blit_tinted` 画一遍纯白：

```cpp
if (c.tinted)
    gfx::blit_tinted(fb_, *c.sheet, c.src, c.x, c.y, 0xFFFFFFFFu);
```

`tint` 的每个通道是"乘数"，`0xFFFFFFFF` 表示不改变颜色。
**等等，那为什么不闪白？**

因为 `blit_tinted` 的实现是"源颜色 × tint"，白色 tint 等于原样。
真正的闪白需要"往白色插值"，不是乘。

这里用一个更简单的办法：**受伤时把 tint 设成 `0xFF8080FF`**
（红/绿通道拉满、蓝通道减半）——效果是"打红了"，比纯白更像受击。

```cpp
// Health::flash > 0 时
gfx::blit_tinted(fb_, *c.sheet, c.src, c.x, c.y, 0xFF6060FFu);
```

### 10.12 验收

```cpp
#include "game/Combat.h"
#include <cstdio>

int main() {
    // 中心在 (100+32, 100+32) = (132,132)
    core::Rect r = game::melee_box(100, 100, 3, 36);   // 朝右
    std::printf("朝右: x=%d y=%d w=%d h=%d（期望 148,116,36,32）\n",
                r.x, r.y, r.w, r.h);

    core::Rect u = game::melee_box(100, 100, 0, 36);   // 朝上
    std::printf("朝上: x=%d y=%d w=%d h=%d（期望 116,96,36,32）\n",
                u.x, u.y, u.w, u.h);

    // 判定：右侧的目标应该被打到，左侧不应该
    const core::Rect target_right{ 155, 120, 32, 20 };
    const core::Rect target_left { 60,  120, 32, 20 };
    std::printf("右侧目标命中: %d（应为 1）\n", core::rect_overlap(r, target_right));
    std::printf("左侧目标命中: %d（应为 0）\n", core::rect_overlap(r, target_left));

    // 冷却
    game::AttackState a;
    a.trigger(2);
    std::printf("触发后 active=%d\n", a.active);
    for (int i = 0; i < 13; ++i) a.update(0.01f);
    std::printf("0.13s 后 active=%d（应为 0）\n", a.active);
    return 0;
}
```

- [ ] 四个方向矩形的数值都对
- [ ] 右侧命中 1，左侧命中 0
- [ ] 0.13 秒后 `active=0`
- [ ] 把 `duration` 改成 0.5，0.13 秒后仍然是 1（说明时长生效）

---

## 阶段 11 · 敌人 AI

### 11.1 不要写 25 个状态机

25 种普通怪的差异只有两类：

1. **数值**：血量、攻击、速度、视野
2. **能力组合**：追、远程、飞、分裂、召唤、冲锋、闪现、站桩、毒、护盾

所以用**行为位**：

```cpp
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
```

一种敌人的行为 = 这些位的或：

```cpp
B::Chase | B::Flying | B::Ranged     // 会飞、会追、会远程
```

`update_enemy` 里就是一系列 `if (behavior & B::Xxx)`。
**加一种敌人只加一行表，不改任何代码。**

### 11.2 要建的文件

```
include/game/Enemy.h
src/game/Enemy.cpp
```

### 11.3 `include/game/Enemy.h`

<!-- FILE: include/game/Enemy.h -->
```cpp
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
```

**注意 `#include "ecs/Registry.h"`，不能只前向声明 `class Registry`**。
因为 `ecs::Entity` 是 `using Entity = uint32_t;` —— **类型别名必须看到定义**，
前向声明拿不到它，报错是 `"Entity": 不是 "ecs" 的成员`。

### 11.4 `src/game/Enemy.cpp`

<!-- FILE: src/game/Enemy.cpp -->
```cpp
#include "game/Enemy.h"
#include "game/Components.h"
#include "gfx/Atlas.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unordered_map>

namespace game {

// ---------------------------------------------------------------
// 配置表。加一种敌人 = 加一行。
//
// 字段顺序：name, sprite, hp, atk, speed,
//           fw, fh, frames, rows, draw_w, draw_h,
//           behavior, sight, [ranged_range], [split_into], [summon_into], [is_boss]
// ---------------------------------------------------------------
const EnemySpec ENEMIES[] = {
    // ---- 人形（LPC 合成，64×64，4 方向 8 帧）----
    {"bandit",   "chars/bandit_walk.png",   60,  8, 70,  64, 64, 8, 4, 48, 48, B::Chase,                200},
    {"cultist",  "chars/cultist_walk.png",  45, 12, 45,  64, 64, 8, 4, 48, 48, B::Chase | B::Ranged,    260, 120},
    {"knight",   "chars/knight_walk.png",  180, 18, 35,  64, 64, 8, 4, 52, 52, B::Chase,                180},

    // ---- 小型怪（原图极小，放大画）----
    {"goblin",   "monsters/goblin.png",     35,  6, 90,  13, 10, 1, 1, 20, 20, B::Chase,                200},
    {"goblin_chief","monsters/goblin.png", 120, 14, 70,  13, 10, 1, 1, 30, 30, B::Chase,                220},
    {"skeleton", "monsters/skeleton.png",   50, 10, 50,  13, 11, 1, 1, 20, 20, B::Chase,                220},
    {"skeleton_archer","monsters/skeleton.png", 40, 9, 45, 13, 11, 1, 1, 22, 22, B::Chase | B::Ranged,   240, 150},
    {"slim",     "monsters/slim.png",       70,  7, 30,  13,  6, 1, 1, 20, 20, B::Chase | B::Split,     160, 0, "slim_small"},
    {"slim_small","monsters/slim.png",      25,  4, 45,  13,  6, 1, 1, 16, 16, B::Chase,                140},
    {"snake",    "monsters/snake.png",      30,  7, 60,  24, 16, 1, 1, 24, 24, B::Chase,                180},
    {"leonard",  "monsters/leonard.png",    90, 15, 55,  51, 27, 1, 1, 40, 40, B::Chase,                200},
    {"cyclop",   "monsters/cyclop.png",    220, 25, 35,  32, 16, 1, 1, 32, 32, B::Chase | B::Charge,    220},
    {"king_skeleton","monsters/king skeleton.png",150,20,40, 32,25,1,1, 40,40, B::Chase | B::Summon,     220, 0, nullptr, "skeleton"},

    // ---- 多帧图集（一行 N 帧）----
    {"bat",      "monsters/bat-1.png",      25,  5, 110, 51, 114, 7, 1, 32, 32, B::Chase | B::Flying,    260},
    {"insect",   "monsters/insect-1.png",   20,  5, 120, 97, 138, 4, 1, 40, 40, B::Chase,                180},
    {"lizard",   "monsters/lizard-1.png",   55,  9, 80, 105, 204, 6, 1, 44, 44, B::Chase,                200},
    {"plant",    "monsters/plant-1.png",    80,  0,  0, 135, 340, 6, 1, 44, 44, B::Static | B::Poison,    60},
    {"ptero",    "monsters/pterodactyl-1.png",45,12, 95, 126, 452, 4, 1, 72, 72, B::Chase | B::Flying,    280},
    {"turtle",   "monsters/turtle-1.png",  300, 10, 20, 360, 152, 1, 1, 56, 56, B::Chase | B::Shield,    150},
    {"mini_tyranno","monsters/mini-tyrannosaurus-1.png",120,18,75, 80, 248, 6, 1, 64, 64, B::Chase,      220},
    {"trex",     "monsters/tyrannosaurus-1.png",250,30,75,151,420, 6, 1, 96, 96, B::Chase | B::Charge,   300},
    {"dragon",   "monsters/dragon-1.png",  400, 25, 50,  98, 260, 4, 1, 96, 96, B::Chase,                400},

    // ---- 异界 ----
    {"ship",     "monsters/1.png",          80, 18, 85,  64, 64, 1, 1, 40, 40, B::Chase | B::Flying | B::Ranged, 300, 140},
    {"ninja",    "chars/extra/ninja/1.png",120, 20,100,  64, 56, 1, 1, 48, 48, B::Chase | B::Blink,     300},
    {"cowboy",   "chars/extra/western/1.png",100,22, 60, 63, 66, 7, 1, 48, 48, B::Chase | B::Ranged,    260, 160},

    // ---- Boss（is_boss = true）----
    {"boss_blight","monsters/king skeleton.png", 1200, 40, 45, 32, 25, 1, 1, 120, 120,
        B::Chase | B::Summon | B::Blink, 400, 0, nullptr, "skeleton", true},
    {"boss_trex", "monsters/tyrannosaurus-1.png", 600, 35, 60, 151, 420, 6, 1, 160, 160,
        B::Chase | B::Charge, 400, 0, nullptr, nullptr, true},
    {"boss_dragon","monsters/dragon-1.png", 800, 30, 55, 98, 260, 4, 1, 150, 150,
        B::Chase | B::Ranged, 420, 200, nullptr, nullptr, true},
};

const int ENEMY_COUNT = int(sizeof(ENEMIES) / sizeof(ENEMIES[0]));

const EnemySpec* find_enemy(const char* name) {
    if (!name) return nullptr;
    for (int i = 0; i < ENEMY_COUNT; ++i)
        if (std::strcmp(ENEMIES[i].name, name) == 0) return &ENEMIES[i];
    return nullptr;
}

// ---------------------------------------------------------------
// 贴图缓存：同一张图只加载一次
// ---------------------------------------------------------------
namespace {

std::unordered_map<std::string, gfx::Surface>& sprite_cache() {
    static std::unordered_map<std::string, gfx::Surface> c;
    return c;
}

} // namespace

const gfx::Surface* load_sheet(const char* rel) {
    auto& c = sprite_cache();
    auto it = c.find(rel);
    if (it == c.end()) {
        gfx::Surface s = gfx::load_png(std::string("assets/gfx/") + rel);
        it = c.emplace(rel, std::move(s)).first;
    }
    return &it->second;
}

ecs::Entity spawn_enemy(ecs::Registry& reg, const char* name, float x, float y) {
    const EnemySpec* spec = find_enemy(name);
    if (!spec) {
        std::fprintf(stderr, "未知敌人类型: %s\n", name);
        return ecs::INVALID_ENTITY;      // 不能用 0：0 是合法实体 id
    }

    const gfx::Surface* sheet = load_sheet(spec->sprite);

    const ecs::Entity e = reg.create();
    reg.add<Transform>(e, x, y);
    reg.add<Health>(e, spec->hp, spec->hp);
    reg.add<AI>(e, spec->behavior, float(spec->speed),
                spec->sight, spec->ranged_range);
    reg.add<EnemyTag>(e, spec);

    Sprite sp(sheet, spec->draw_w, spec->draw_h);
    sp.anim.setup(sheet, spec->fw, spec->fh, spec->frames, spec->rows);
    reg.add<Sprite>(e, sp);

    if (spec->behavior & B::Flying) reg.add<Flying>(e);
    if (spec->is_boss)              reg.add<Boss>(e, 1);

    // 错开首次开火时间，否则一进地图就被齐射
    reg.get<AI>(e)->attack_cd = float(std::rand() % 200) / 100.f;

    return e;
}

ecs::Entity spawn_projectile(ecs::Registry& reg, float x, float y,
                             float vx, float vy, int dmg,
                             bool from_player, bool pierce) {
    const ecs::Entity e = reg.create();
    reg.add<Transform>(e, x, y);
    reg.add<Velocity>(e, vx, vy);
    reg.add<Projectile>(e, dmg, 1.8f, pierce, from_player);
    return e;
}

ecs::Entity spawn_fx(ecs::Registry& reg, float x, float y, int pool_size) {
    const ecs::Entity e = reg.create();
    reg.add<Transform>(e, x, y);
    // 从特效池里随机挑一张，不然 85 张图永远只用第 0 张
    const int frame = (pool_size > 0) ? (std::rand() % pool_size) : 0;
    reg.add<Fx>(e, frame, 0.35f);
    return e;
}

ecs::Entity drop_item(ecs::Registry& reg, float x, float y, int pool_size) {
    if (pool_size <= 0) return 0;
    // 只掉 65%，不然地上全是垃圾
    if (std::rand() % 100 >= 65) return 0;

    const ecs::Entity e = reg.create();
    reg.add<Transform>(e, x, y);
    reg.add<Pickup>(e, std::rand() % pool_size, std::rand() % 3);
    return e;
}

// ---------------------------------------------------------------
const Weapon WEAPONS[] = {
    // name          kind                  dmg  range  cd     ammo pierce
    {"rusty_sword", Weapon::Kind::Melee,     12,  28.f, 0.35f,  -1, false},
    {"smith_sword", Weapon::Kind::Melee,     20,  36.f, 0.32f,  -1, false},
    {"flintlock",   Weapon::Kind::Ranged,    35, 420.f, 0.80f,  24, false},
    {"laser",       Weapon::Kind::Laser,     50, 620.f, 0.50f,  12, true},
    {"bone_club",   Weapon::Kind::Melee,     15,  32.f, 0.45f,  -1, false},
};

const int WEAPON_COUNT = int(sizeof(WEAPONS) / sizeof(WEAPONS[0]));

} // namespace game
```

**这张表就是"素材全部用上"的落点**。
25 行普通怪 + 3 行 Boss，每行对应一个素材文件，
`.map` 里写 `enemy_goblin 18 24` 就生成一只。

**为什么 `fw/fh/frames/rows` 要分开写**：
`monsters/bat-1.png` 是 357×114，7 帧 → `fw=51, fh=114`；
`monsters/goblin.png` 是 13×10 单帧 → `fw=13, fh=10`。
如果按"所有人都是 64×64"去取帧，哥布林会取到一片空白。
`draw_w/draw_h` 再把它缩放到 20×20，视觉上才和 48×48 的人形协调。

### 11.5 主循环：一个函数管 25 种敌人

```cpp
void Game::update_enemies(float dt) {
    reg_.each<Transform, Health, AI>([&](ecs::Entity e, Transform& t, Health& h, AI& ai) {
        // 1) 死亡处理
        if (h.hp <= 0) {
            on_enemy_death(e, t);          // 只需要实体和位置
            return;
        }
        if (ai.stun > 0.f) { ai.stun -= dt; return; }

        // 2) 到玩家的方向和距离
        const float dx = player_.x - t.x;
        const float dy = player_.y - t.y;
        const float d  = std::sqrt(dx*dx + dy*dy) + 1e-5f;   // 防止除零

        // 3) 站桩怪：不移动，但可以远程
        if (ai.behavior & B::Static) { /* 落到远程分支 */ }
        else if (d < float(ai.sight) && (ai.behavior & B::Chase)) {
            const float stop = (ai.behavior & B::Ranged) ? 70.f : 0.f;
            if (d > stop) {
                const float vx = dx / d * ai.speed * dt;
                const float vy = dy / d * ai.speed * dt;
                world::try_move(t.x, t.y, vx, vy, map_,
                                (ai.behavior & B::Flying) != 0);
            }
        }

        // 4) 远程：进入射程且冷却好了就射
        if ((ai.behavior & B::Ranged) && ai.ranged_range > 0 &&
            d < float(ai.ranged_range)) {
            if (ai.attack_cd <= 0.f) {
                ai.attack_cd = 1.6f;
                spawn_projectile(reg_, t.x + 24, t.y + 24,
                                 dx / d * 240.f, dy / d * 240.f,
                                 6, false, false);
            }
        }
        if (ai.attack_cd > 0.f) ai.attack_cd -= dt;

        // 5) 闪现
        if (ai.behavior & B::Blink) {
            ai.blink_cd += dt;
            if (ai.blink_cd >= 3.f && d < 400.f) {
                ai.blink_cd = 0.f;
                const float jx = t.x + dx / d * 80.f;
                const float jy = t.y + dy / d * 80.f;
                if (!world::box_solid(world::feet(jx, jy), map_)) {
                    t.x = jx; t.y = jy;
                }
            }
        }

        // 6) 接触伤害
        const core::Rect a{ int(t.x) + 16, int(t.y) + 40, 32, 20 };
        if (core::rect_overlap(a, world::feet(player_.x, player_.y)))
            player_.take_damage(1);
    });
}
```

> 上面是**简化版**，只保留"追 + 远程 + 闪现 + 接触伤害"四条主线。
> 完整版还多了三块：Boss 阶段、精灵朝向、冲锋加速——
> 都在 `03e` 的 `Game.cpp` 里，代码逐行都在，照着抄就行。

**注意第 4 步的 `ai.ranged_range`**：
`AI` 组件里为什么要存射程？因为实体不知道自己是从哪一行表生成的。
两种解法：

1. 生成时把射程拷进 `AI` —— 加一个 `int ranged_range` 字段。
2. 给实体加 `EnemyTag { const EnemySpec* spec; }`，每次查表。

**数值用第 1 种，身份用第 2 种。**
射程是"每帧都要用"的数值，拷进组件避免查表；
`split_into / is_boss` 只在死亡时用一次，存指针更省。

这两个字段在 03b 的 `Components.h` 里已经写好了
（`AI::ranged_range` 和 `EnemyTag`），直接用。

### 11.6 死亡处理：分裂、召唤、掉落

死亡处理要干四件事，顺序不能反：

```
1. 查"我是谁"（EnemySpec）→ 决定分裂/召唤/Boss 判定
2. 分裂、召唤（要读 t，必须在 destroy 之前）
3. 掉落 + 特效（也要读 t）
4. reg_.destroy(e)   ← 永远最后一行
```

**但这里有个问题：实体不知道自己的 `EnemySpec`**。
死亡处理需要 `split_into / summon_into`，这是"身份"信息，不是数值。

所以 `EnemyTag`（03b 的 `Components.h` 里已定义）就是干这个的：

```cpp
// 03b 已经写进 Components.h 了，这里是回顾
struct EnemyTag {
    const EnemySpec* spec = nullptr;
    EnemyTag() = default;
    explicit EnemyTag(const EnemySpec* s) : spec(s) {}
};
```

`spawn_enemy` 里 `reg.add<EnemyTag>(e, spec)`（上面已经加了）。

死亡处理：

```cpp
void Game::on_enemy_death(ecs::Entity e, Transform& t) {
    const EnemyTag* tag = reg_.get<EnemyTag>(e);
    const EnemySpec* spec = tag ? tag->spec : nullptr;

    if (spec) {
        if (spec->split_into)
            for (int i = 0; i < 2; ++i)
                spawn_enemy(reg_, spec->split_into, t.x + i * 24.f - 12.f, t.y);

        if (spec->summon_into && (std::rand() % 100) < 40)
            spawn_enemy(reg_, spec->summon_into, t.x + 40.f, t.y);

        if (spec->is_boss) {
            quest_.boss_defeated = true;
            if (std::strcmp(spec->name, "boss_blight") == 0) quest_.ash_key = true;
            if (std::strcmp(spec->name, "boss_trex")   == 0) quest_.primal_blood = true;
            if (std::strcmp(spec->name, "boss_dragon") == 0) {
                state_ = State::Ending;
                ending_t_ = 0.f;
            }
        }
    }

    drop_item(reg_, t.x, t.y, int(g_items.size()));
    spawn_fx(reg_, t.x, t.y, int(g_fx.size()));   // 从 85 张特效里随机挑一张
    reg_.destroy(e);
}
```

**为什么分裂要在 `on_enemy_death` 里立刻做**：
如果延迟一帧，`reg_.destroy(e)` 已经清掉组件，`t` 就是悬空引用。
`each` 的 lambda 参数是引用，`destroy` 之后不能再用。

**`reg_.destroy(e)` 之后不能再用 `t / h / ai`**——它们是池子里那块内存的引用。
`destroy` 只是清掩码，内存还在，所以"读"通常不会崩，但值是垃圾。
养成习惯：`destroy` 永远放最后一行。

### 11.7 主动性：别让敌人一起扑上来

所有怪同时追你，体验是"被淹没"。
两个廉价的办法：

1. **视野限制**：`d < sight` 才开始追。`sight=200` 就是 6 格。
2. **错开攻击冷却**：生成时给 `attack_cd` 一个随机初值，别让远程怪齐射。

```cpp
// spawn_enemy 结尾
reg.add<AI>(e, ...);
reg.get<AI>(e)->attack_cd = float(std::rand() % 100) / 100.f * 2.f;
```

第 2 条很容易忘，忘了的症状是"一进地图就被 4 发子弹同时打中"。

### 11.8 验收

```cpp
#include "ecs/Registry.h"
#include "game/Enemy.h"
#include "game/Components.h"
#include <cstdio>

int main() {
    std::printf("配置表 %d 行\n", game::ENEMY_COUNT);

    // 每一行的素材都要能打开
    ecs::Registry reg;
    int bad = 0;
    for (int i = 0; i < game::ENEMY_COUNT; ++i) {
        const auto& s = game::ENEMIES[i];
        const ecs::Entity e = game::spawn_enemy(reg, s.name, 0, 0);
        if (e == ecs::INVALID_ENTITY) { std::printf("  失败: %s\n", s.name); ++bad; continue; }
        auto* sp = reg.get<game::Sprite>(e);
        if (!sp || !sp->sheet) { std::printf("  无贴图: %s\n", s.name); ++bad; }
        else if (sp->anim.fw * sp->anim.frames > sp->sheet->width()) {
            std::printf("  帧数超宽: %s  fw=%d frames=%d 宽=%d\n",
                        s.name, sp->anim.fw, sp->anim.frames, sp->sheet->width());
            ++bad;
        }
    }
    std::printf("生成失败 %d 个\n", bad);
    return 0;
}
```

- [ ] `配置表 28 行`
- [ ] `生成失败 0 个`（每张图都能打开）
- [ ] 没有"帧数超宽"（`fw × frames ≤ 图片宽度`）
- [ ] 把 `goblin` 的 `fw` 改成 64，会报"帧数超宽"——说明检查有效

---

## 这一册结束后你应该有什么

```
include/game/{Combat,Player,Inventory,Enemy}.h
src/game/Enemy.cpp
```

**现在你应该能做到**：挥剑打怪、开枪、敌人追你并还击、
被打死后分裂/召唤/掉落。所有敌人行为都由 `ENEMIES[]` 那张表决定。
