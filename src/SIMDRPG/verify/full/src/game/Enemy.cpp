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
