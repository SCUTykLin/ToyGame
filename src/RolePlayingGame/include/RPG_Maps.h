//
// Created by YangKai.Lin on 2026/9/30.
//

#ifndef TOYPROJECTS_RoleGame_RPG_MAPS_H
#define TOYPROJECTS_RoleGame_RPG_MAPS_H

#include <string>
#include "olcConsoleGameEngine.h"

namespace RoleGame {
    class cMap {
    public:
        cMap();
        ~cMap();

        int nWidth;
        int nHeight;
        std::string sName;
        olcSprite *pSprite;
        int GetIndex(int x, int y);
        bool GetSolid(int x, int y);
        bool Create(const std::string& fileData, olcSprite *sprite, std::string name);

    private:
        int *m_indices = nullptr;
        bool *m_solids = nullptr;
    };

    class cMap_Village1 : public cMap {
    public:
        cMap_Village1();
    };
}

#endif //TOYPROJECTS_RoleGame_RPG_MAPS_H
