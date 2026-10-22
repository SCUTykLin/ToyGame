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
