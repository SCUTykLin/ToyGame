#pragma once


#include <iostream>
#include <string>
#include <memory.h>
#include <algorithm>

#include "olcConsoleGameEngine.h"

#include "RPG_Assets.h"
#include "RPG_Maps.h"
#include "RPG_Dynamic.h"
#include "RPG_Command.h"
#include "RPG_Quests.h"
#include "RPG_Items.h"

#define X(n) m_script.AddCommand(new cCommand_ ## n)

namespace RoleGame {
    class RPG_Engine : public olcConsoleGameEngine {
    public:
        RPG_Engine();
    private:
        RoleGame::cMap *m_pCurrentMap = nullptr;
        RoleGame::cDynamic_Creature *m_pPlayer = nullptr;
        std::vector<RoleGame::cDynamic *> m_vecDynamics; // Fixed
        std::vector<RoleGame::cDynamic *> m_vecProjectiles; // Transient
        cScriptProcessor m_script;

        std::list<cQuest *> m_listQuests;
        std::list<cItem *> m_listItems;

        float fCameraPosX = 0.0f;
        float fCameraPosY = 0.0f;

        olcSprite *m_sprFont = nullptr;

        enum {
            MODE_TITLE,
            MODE_LOCAL_MAP,
            MODE_WORLD_MAP,
            MODE_INVENTORY,
            MODE_SHOP
        };

        int m_nGameMode = MODE_LOCAL_MAP;

        int m_nInvSelectX = 0;
        int m_nInvSelectY = 0;

    protected:
        bool OnUserCreate() override;

        bool OnUserUpdate(float fElapsedTime) override;

        //bool UpdateTitleScreen(float fElapsedTime);
        bool UpdateLocalMap(float fElapsedTime);

        //bool UpdateWorldMap(float fElapsedTime);
        bool UpdateInventory(float fElapsedTime);

        //bool UpdateShop(float fElapsedTime);

    protected:
        std::vector<std::string> m_vecDialogToShow;
        bool m_bShowDialog = false;
        float m_fDialogX = 0.0f;
        float m_fDialogY = 0.0f;

    public:
        void ShowDialog(std::vector<std::string> vecLines);

        void DisplayDialog(std::vector<std::string> vecText, int x, int y);

        void DrawBigText(std::string sText, int x, int y);

        void ChangeMap(std::string sMapName, float x, float y);

        void AddQuest(cQuest *quest);

        void AddProjectile(cDynamic_Projectile *proj);

        bool GiveItem(cItem *item);

        bool TakeItem(cItem *item);

        bool HasItem(cItem *item);

        void Attack(cDynamic_Creature *aggressor, cWeapon *weapon);

        void Damage(cDynamic_Projectile *projectile, cDynamic_Creature *victim);
    };
}