//
// Created by YangKai.Lin on 2026/10/1.
//

#include "RPG_Assets.h"
#include <string>

namespace RoleGame {
    RPG_Assets::RPG_Assets() {}

    RPG_Assets::~RPG_Assets() {}

    void RPG_Assets::LoadSprites() {
        auto load = [&](const std::string& sName, const std::wstring& sFileName) {
            olcSprite* s = new olcSprite(sFileName);
            m_mapSprites[sName] = s;
        };

        load("village", L"src/RolePlayingGame/assets/gfx/toml_spritesheetdark.spr");
        load("skelly", L"src/RolePlayingGame/assets/gfx/toml_Char001.png.spr");
        load("player", L"src/RolePlayingGame/assets/gfx/toml_CharacterSprites.spr");
        load("font", L"src/RolePlayingGame/assets/gfx/javidx9_nesfont8x8.spr");
        load("worldmap", L"src/RolePlayingGame/assets/gfx/worldmap1.png.spr");
        load("skymap", L"src/RolePlayingGame/assets/gfx/sky1.png.spr");
        load("title", L"src/RolePlayingGame/assets/gfx/title3.png.spr");
        load("balloon", L"src/RolePlayingGame/assets/gfx/balloon1.png.spr");
        load("sword", L"src/RolePlayingGame/assets/gfx/Sword.spr");
        load("hitech", L"src/RolePlayingGame/assets/gfx/toml_modernish.spr");
        load("purple", L"src/RolePlayingGame/assets/gfx/toml_purple.spr");
        load("health", L"src/RolePlayingGame/assets/gfx/item_health.spr");
        load("healthboost", L"src/RolePlayingGame/assets/gfx/item_healthboost.spr");
        load("Basic Sword", L"src/RolePlayingGame/assets/gfx/weapon_basic_sword.spr");
    }

    void RPG_Assets::LoadMaps() {
        auto load = [&](cMap* m) {
            m_mapMaps[m->sName] = m;
        };

        load(new cMap_Village1());
        load(new cMap_Home1());
    }
}
