//
// Created by YangKai.Lin on 2026/9/30.
//

#ifndef TOYPROJECTS_RoleGame_RPG_MAPS_H
#define TOYPROJECTS_RoleGame_RPG_MAPS_H

#include <string>
#include "olcConsoleGameEngine.h"
#include "RPG_Assets.h"
#include "RPG_Dynamic.h"
#include "RPG_Command.h"

namespace RoleGame {
    class cMap {
    public:
        enum NATURE {
            TALK,
            WALK
        };

    public:
        cMap();

        virtual ~cMap();

    public:
        int nWidth;
        int nHeight;
        std::string sName;
        olcSprite *pSprite;

        int GetIndex(int x, int y);

        bool GetSolid(int x, int y);

        bool Create(std::string fileData, olcSprite *sprite, std::string name);

        virtual bool PopulateDynamics(std::vector<cDynamic *> &vecDyns) {
            return false;
        }

        virtual bool OnInteraction(std::vector<cDynamic *> &vecDynobs, cDynamic *target, NATURE nature) {
            return false;
        }

    private:
        int *m_indices = nullptr;
        bool *m_solids = nullptr;

    public:
        static cScriptProcessor *g_script;
    };


    class cMap_Village1 : public cMap {
    public:
        cMap_Village1();

        bool PopulateDynamics(std::vector<cDynamic *> &vecDyns) override;

        bool OnInteraction(std::vector<cDynamic *> &vecDynobs, cDynamic *target, NATURE nature) override;
    };

    class cMap_Home1 : public cMap {
    public:
        cMap_Home1();

        bool PopulateDynamics(std::vector<cDynamic *> &vecDyns) override;
        bool OnInteraction(std::vector<cDynamic *> &vecDynobs, cDynamic *target, NATURE nature) override;
    };
}

#endif //TOYPROJECTS_RoleGame_RPG_MAPS_H
