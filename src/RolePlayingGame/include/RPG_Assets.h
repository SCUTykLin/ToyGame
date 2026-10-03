//
// Created by YangKai.Lin on 2026/10/1.
//

#ifndef TOYPROJECTS_RoleGame_RPG_ASSETS_H
#define TOYPROJECTS_RoleGame_RPG_ASSETS_H

#include "olcConsoleGameEngine.h"
#include <map>
#include <string>
#include "RPG_Maps.h"

namespace RoleGame {
    class RPG_Assets {
    public:

        static RPG_Assets& get() {
            static RPG_Assets me;
            return me;
        }

        RPG_Assets(RPG_Assets const&) = delete;
        void operator=(RPG_Assets const&) = delete;

        olcSprite* GetSprite(const std::string& name) {
            return m_mapSprites[name];
        }

        cMap* GetMap(const std::string& name) {
            return m_mapMaps[name];
        }

        void LoadSprites();
        void LoadMaps();

    private:
        RPG_Assets();
        ~RPG_Assets();

        std::map<std::string, olcSprite*> m_mapSprites;
        std::map<std::string, cMap*> m_mapMaps;
    };
}
#endif //TOYPROJECTS_RoleGame_RPG_ASSETS_H
