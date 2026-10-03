//
// Created by YangKai.Lin on 2026/9/30.
//

#ifndef TOYPROJECTS_RoleGame_RPG_MAPS_H
#define TOYPROJECTS_RoleGame_RPG_MAPS_H

#include <string>
#include "RPG_Dynamic.h"
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

        virtual bool PopulateDynamic(std::vector<cDynamic*>& vecDyns) {
            return false;
        }

    private:
        int *m_indices = nullptr;
        bool *m_solids = nullptr;
    };

    class cMap_Village1 : public cMap {
    public:
        cMap_Village1();
        bool PopulateDynamic(std::vector<cDynamic*>& vecDyns) override;
    };

    class cMap_Home1 : public cMap {
    public:
        cMap_Home1();
        bool PopulateDynamic(std::vector<cDynamic*>& vecDyns) override;
    };
}

#endif //TOYPROJECTS_RoleGame_RPG_MAPS_H
