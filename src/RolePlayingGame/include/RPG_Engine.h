//
// Created by YangKai.Lin on 2026/10/3.
//

#ifndef TOYPROJECTS_RoleGame_RPG_ENGINE_H
#define TOYPROJECTS_RoleGame_RPG_ENGINE_H

#include <iostream>
#include <string>
#include <memory.h>
#include <algorithm>
#include "RPG_Assets.h"
#include <vector>
#include "RPG_Maps.h"
#include "RPG_Dynamic.h"
#include "RPG_Command.h"
#include "olcConsoleGameEngine.h"

#define X(n) m_script.AddCommand(new cCommand_ ## n)

using namespace RoleGame;

class RPG_Engine : public olcConsoleGameEngine
{
public:
    RPG_Engine();

private:
    cMap *m_pCurrentMap = nullptr;
    cDynamic_Creature *m_pPlayer = nullptr;
    std::vector<cDynamic*> m_vecDynamics;    // Fixed
    std::vector<cDynamic*> m_vecProjectiles; // Transient
    cScriptProcessor m_script;

    float fCameraPosX = 0.0f;
    float fCameraPosY = 0.0f;

    olcSprite *m_sprFont = nullptr;

    enum
    {
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
    void DrawBigText(const std::string &sText, int x, int y);
    void ChangeMap(const std::string& sMapName, float x, float y);
};

#endif //TOYPROJECTS_RoleGame_RPG_ENGINE_H
