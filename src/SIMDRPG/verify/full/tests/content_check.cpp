// 内容自检（不是讲义的一部分，是仓库的验证程序）
//
// 检查三件事：
//   1. 六张地图都能加载，且地图里引用的敌人都存在于 ENEMIES[]
//   2. ENEMIES[] 的 28 行都能生成实体，帧宽/帧高不超过素材尺寸
//   3. 五道门禁的旗标链正确
//
// 构建（在 verify/full 目录下，先把参考实现构建过一次）：
//   cl /utf-8 /std:c++20 /DSDL_MAIN_HANDLED /EHsc /Iinclude ^
//      /I../../Thirdparty/SDL2/SDL-release-2.32.6/include ^
//      /I../../Thirdparty/SDL2/SDL_image-release-2.8.0/include ^
//      tests/content_check.cpp src/world/TileMap.cpp src/gfx/Surface.cpp ^
//      src/gfx/Atlas.cpp src/game/Enemy.cpp /Fe:content_check.exe ^
//      /link <SDL2 的两个 .lib 和系统库>
//
// 运行（必须在仓库根目录）：
//   ./verify/full/tests/content_check.exe
//
#include "world/TileMap.h"
#include "game/Enemy.h"
#include "game/Quest.h"
#include "ecs/Registry.h"
#include "game/Components.h"
#include <cstdio>
#include <cstring>
#include <string>

int main() {
    int fails = 0;

    // 1) 六张地图
    const char* names[] = {"village","forest","pass","crypt","nest","rift"};
    ecs::Registry reg;
    int total_entities = 0;
    for (const char* n : names) {
        world::TileMap m = world::TileMap::load(std::string("assets/maps/") + n + ".map");
        total_entities += int(m.spawns.size());
        // 地图里的每个 enemy_/boss_ 都能在表里找到
        for (const auto& s : m.spawns) {
            const char* key = nullptr;
            if (s.type.rfind("enemy_", 0) == 0) key = s.type.c_str() + 6;
            else if (s.type.rfind("boss_", 0) == 0) key = s.type.c_str();
            if (!key) continue;
            if (!game::find_enemy(key)) {
                std::printf("  地图 %s 引用了未知敌人: %s\n", n, key);
                ++fails;
            }
        }
    }
    std::printf("[1] 六张地图加载完成，实体总数 %d\n", total_entities);

    // 2) 28 行配置表全部可生成
    int spawned = 0;
    for (int i = 0; i < game::ENEMY_COUNT; ++i) {
        const auto& s = game::ENEMIES[i];
        const ecs::Entity e = game::spawn_enemy(reg, s.name, 0.f, 0.f);
        if (e == ecs::INVALID_ENTITY) { std::printf("  生成失败: %s\n", s.name); ++fails; continue; }
        auto* sp = reg.get<game::Sprite>(e);
        if (!sp || !sp->sheet) { std::printf("  无贴图: %s\n", s.name); ++fails; continue; }
        if (sp->anim.fw * sp->anim.frames > sp->sheet->width()) {
            std::printf("  帧宽越界: %s\n", s.name); ++fails;
        }
        if (sp->anim.fh > sp->sheet->height()) {
            std::printf("  帧高越界: %s\n", s.name); ++fails;
        }
        ++spawned;
    }
    std::printf("[2] 配置表 %d 行，成功生成 %d 个\n", game::ENEMY_COUNT, spawned);

    // 3) 门禁链：从零开始，逐个旗标解锁
    {
        game::QuestFlags q;
        world::EntitySpawn forest; forest.target_map = "forest";
        world::EntitySpawn pass;   pass.target_map = "pass";
        world::EntitySpawn crypt;  crypt.target_map = "crypt";
        world::EntitySpawn nest;   nest.target_map = "nest";
        world::EntitySpawn rift;   rift.target_map = "rift";

        if (game::can_use_portal(forest, q)) { std::printf("  没剑就能进森林\n"); ++fails; }
        q.has_sword = true;
        if (!game::can_use_portal(forest, q)) { std::printf("  有剑进不去森林\n"); ++fails; }
        if (game::can_use_portal(pass, q)) { std::printf("  没森之核就能进山道\n"); ++fails; }
        q.heartseed = true; q.frost_tear = true;
        if (!game::can_use_portal(pass, q) || !game::can_use_portal(crypt, q)) {
            std::printf("  森之核/霜之泪没解锁\n"); ++fails;
        }
        if (game::can_use_portal(nest, q)) { std::printf("  没钥匙就能进兽巢\n"); ++fails; }
        q.ash_key = true;
        if (!game::can_use_portal(nest, q)) { std::printf("  有钥匙进不去兽巢\n"); ++fails; }
        if (game::can_use_portal(rift, q)) { std::printf("  没血就能进裂隙\n"); ++fails; }
        q.primal_blood = true;
        if (!game::can_use_portal(rift, q)) { std::printf("  有血进不去裂隙\n"); ++fails; }
        std::printf("[3] 门禁链 5 道全部正确\n");
    }

    std::printf("\n失败 %d 项\n", fails);
    return fails ? 1 : 0;
}
