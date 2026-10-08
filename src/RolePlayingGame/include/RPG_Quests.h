//
// Created by YangKai.Lin on 2026/10/8.
//

#ifndef TOYPROJECTS_RPG_QUESTS_H
#define TOYPROJECTS_RPG_QUESTS_H
#pragma once

#include "RPG_Dynamic.h"
#include "RPG_Command.h"

namespace RoleGame {
    class RPG_Engine;
    class cQuest {
    public:
        enum NATURE {
            TALK = 0,
            ATTACK = 1,
            KILL = 2,
            WALK = 3
        };

    public:
        cQuest();

    public:
        virtual bool OnInteraction(std::vector<RoleGame::cDynamic *> &vecDynobs, RoleGame::cDynamic *target, NATURE nature);

        virtual bool PopulateDynamics(std::vector<RoleGame::cDynamic *> &vecDyns, std::string sMap);

    public:
        std::string sName;
        bool bCompleted = false;
        static cScriptProcessor *g_script;
        static RPG_Engine *g_engine;
    };


    class cQuest_MainQuest : public cQuest {
    public:
        bool PopulateDynamics(std::vector<cDynamic *> &vecDyns, std::string sMap) override;

        bool OnInteraction(std::vector<cDynamic *> &vecDynobs, cDynamic *target, NATURE nature) override;

    private:
        int m_nPhase = 0;
    };


    class cQuest_BobsQuest : public cQuest {
    public:
        bool PopulateDynamics(std::vector<cDynamic *> &vecDyns, std::string sMap) override;

        bool OnInteraction(std::vector<cDynamic *> &vecDynobs, cDynamic *target, NATURE nature) override;

    private:
        int m_nPhase = 0;
    };
}
#endif //TOYPROJECTS_RPG_QUESTS_H
