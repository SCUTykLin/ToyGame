//
// Created by YangKai.Lin on 2026/9/30.
//

#include "RPG_Maps.h"
#include <fstream>

#include "RPG_Assets.h"
RoleGame::cScriptProcessor *RoleGame::cMap::g_script = nullptr;
#define X(n) g_script->AddCommand(new cCommand_ ## n)

namespace RoleGame {
    cMap::cMap() {
        pSprite = nullptr;
        nWidth = 0;
        nHeight = 0;
        m_solids = nullptr;
        m_indices = nullptr;
    }

    cMap::~cMap() {
        delete[] m_solids;
        delete[] m_indices;
    }


    bool cMap::GetSolid(int x, int y) {
        if (x >= 0 && x < nWidth && y >= 0 && y < nHeight) return m_solids[y * nWidth + x];
        else return true;
    }

    int cMap::GetIndex(int x, int y) {
        if (x >= 0 && x < nWidth && y >= 0 && y < nHeight) {
            return m_indices[y * nWidth + x];
        } else {
            return 0;
        }
    }

    bool cMap::Create(std::string fileData, olcSprite *sprite, std::string name) {
        sName = name;
        pSprite = sprite;
        std::ifstream data(fileData, std::ios::in | std::ios::binary);
        if (data.is_open()) {
            data >> nWidth >> nHeight;
            m_solids = new bool[nWidth * nHeight];
            m_indices = new int[nWidth * nHeight];
            for (int i = 0; i < nWidth * nHeight; i++) {
                data >> m_indices[i];
                data >> m_solids[i];
            }
            return true;
        }

        return false;
    }


    cMap_Village1::cMap_Village1() {
        Create("src/RolePlayingGame/assets/map/village1.lvl", RPG_Assets::get().GetSprite("village"), "coder town");
    }

    bool cMap_Village1::PopulateDynamics(std::vector<cDynamic *> &vecDyns) {
        // Add Teleporters
        vecDyns.push_back(new cDynamic_Teleport(12.0f, 6.0f, "home", 5.0f, 12.0f));

        // Add Items
        vecDyns.push_back(new cDynamic_Item(10, 10, RPG_Assets::get().GetItem("Small Health")));
        vecDyns.push_back(new cDynamic_Item(12, 10, RPG_Assets::get().GetItem("Health Boost")));

        for (int i = 0; i < 3; i++) {
            cDynamic *g1 = new cDynamic_Creature_Skelly();
            vecDyns.push_back(g1);
            g1->px = rand() % 10 + 5.0f;
            g1->py = rand() % 10 + 5.0f;
        }

        return true;
    }

    bool cMap_Village1::OnInteraction(std::vector<cDynamic *> &vecDynobs, cDynamic *target, NATURE nature) {
        if (target->sName == "Teleport")
            g_script->AddCommand(new cCommand_ChangeMap(
                ((cDynamic_Teleport *) target)->sMapName,
                ((cDynamic_Teleport *) target)->fMapPosX,
                ((cDynamic_Teleport *) target)->fMapPosY));

        return false;
    }

    cMap_Home1::cMap_Home1() {
        Create("src/RolePlayingGame/assets/map/home.lvl", RPG_Assets::get().GetSprite("hitech"), "home");
    }

    bool cMap_Home1::PopulateDynamics(std::vector<cDynamic *> &vecDyns) {
        // Front door
        vecDyns.push_back(new cDynamic_Teleport(5.0f, 13.0f, "coder town", 12.0f, 7.0f));
        vecDyns.push_back(new cDynamic_Teleport(4.0f, 13.0f, "coder town", 12.0f, 7.0f));

        /*cDynamic_Creature* c1 = new cDynamic_Creature("bob", RPG_Assets::get().GetSprite("skelly"));
        c1->px = 12.0f;
        c1->py = 4.0f;
        vecDyns.push_back(c1);*/

        return true;
    }


    bool cMap_Home1::OnInteraction(std::vector<cDynamic *> &vecDynobs, cDynamic *target, NATURE nature) {
        if (target->sName == "Teleport")
            g_script->AddCommand(new cCommand_ChangeMap(
                ((cDynamic_Teleport *) target)->sMapName,
                ((cDynamic_Teleport *) target)->fMapPosX,
                ((cDynamic_Teleport *) target)->fMapPosY));

        /*if (target->sName == "bob")
        {
            X(ShowDialog({ "Hello!", "I'm Bob!" }));
        }*/

        return false;
    }
}
