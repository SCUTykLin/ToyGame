#pragma once
#include "world/TileMap.h"

namespace game {

struct QuestFlags {
    bool met_elder = false;
    bool has_sword = false;

    bool heartseed    = false;   // 森之核（长老给）
    bool frost_tear   = false;   // 霜之泪（进山道时找到）
    bool ash_key      = false;   // 灰烬钥匙（地窖 Boss 掉）
    bool primal_blood = false;   // 原始之血（兽巢 Boss 掉）

    bool boss_defeated = false;
};

// 门禁：没拿到对应圣物，传送门不工作
inline bool can_use_portal(const world::EntitySpawn& s, const QuestFlags& q) {
    if (s.target_map == "forest" && !q.has_sword)    return false;
    if (s.target_map == "pass"   && !q.heartseed)    return false;
    if (s.target_map == "crypt"  && !q.frost_tear)   return false;
    if (s.target_map == "nest"   && !q.ash_key)      return false;
    if (s.target_map == "rift"   && !q.primal_blood) return false;
    return true;
}

// 被挡住时给玩家看的话。每道门单独写，别给一句笼统的"你不能过去"
inline const char* portal_blocked_reason(const world::EntitySpawn& s,
                                        const QuestFlags& q) {
    if (s.target_map == "forest" && !q.has_sword)
        return "得先有把剑。去找铁匠 Brann。";
    if (s.target_map == "pass" && !q.heartseed)
        return "长老说过，森之核能护住山路。先回去找他。";
    if (s.target_map == "crypt" && !q.frost_tear)
        return "地窖的寒气会冻碎骨头。你需要霜之泪。";
    if (s.target_map == "nest" && !q.ash_key)
        return "兽巢的门是烧过的铁。需要灰烬钥匙。";
    if (s.target_map == "rift" && !q.primal_blood)
        return "裂隙外面没有地面。原始之血能让你站住。";
    return "过不去。";
}

} // namespace game
