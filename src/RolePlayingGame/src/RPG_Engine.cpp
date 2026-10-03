//
// Created by YangKai.Lin on 2026/10/3.
//

#include "RPG_Engine.h"

RPG_Engine::RPG_Engine()
{
    m_sAppName = L"Role Play Game";
}

bool RPG_Engine::OnUserCreate() {
    cCommand::g_engine = this;

    RPG_Assets::get().LoadSprites();
    RPG_Assets::get().LoadMaps();

    m_sprFont = RPG_Assets::get().GetSprite("font");

    m_pCurrentMap = RPG_Assets::get().GetMap("coder town");

    m_pPlayer = new cDynamic_Creature("player", RPG_Assets::get().GetSprite("player"));
    m_pPlayer->px = 5.0f;
    m_pPlayer->py = 5.0f;

    m_vecDynamics.push_back(m_pPlayer);

    return true;
}

void RPG_Engine::DrawBigText(const std::string &sText, int x, int y) {
    int i = 0;
    for (auto c: sText) {
        int sx = ((c - 32) % 16) * 8;
        int sy = ((c - 32) / 16) * 8;
        DrawPartialSprite(x + i * 8, y, m_sprFont, sx, sy, 8, 8);
        i++;
    }
}

void RPG_Engine::ChangeMap(const std::string &sMapName, float x, float y) {
    m_vecDynamics.clear();
    m_vecDynamics.push_back(m_pPlayer);
    m_pCurrentMap = RPG_Assets::get().GetMap(sMapName);
    m_pPlayer->px = x;
    m_pPlayer->py = y;

    m_pCurrentMap->PopulateDynamic(m_vecDynamics);
}

bool RPG_Engine::OnUserUpdate(float fElapsedTime) {
        m_script.ProcessCommands(fElapsedTime);
        if (m_script.bUserControlEnabled) { // 没跑完剧本之前不能操作
            m_pPlayer->vx = 0.0f;
            m_pPlayer->vy = 0.0f;
            // Handle Input
            if (IsFocused()) {
                if (GetKey(VK_UP).bHeld) {
                    m_pPlayer->vy = -4.0f;
                }

                if (GetKey(VK_DOWN).bHeld) {
                    m_pPlayer->vy = 4.0f;
                }

                if (GetKey(VK_LEFT).bHeld) {
                    m_pPlayer->vx = -4.0f;
                }

                if (GetKey(VK_RIGHT).bHeld) {
                    m_pPlayer->vx = 4.0f;
                }

                if (GetKey(L'Z').bReleased) {
                    X(MoveTo(m_pPlayer, 10, 10, 3.0f));
                    X(MoveTo(m_pPlayer, 15, 10, 3.0f));
                    X(MoveTo(m_vecDynamics[1], 15, 12, 2.0f));
                    X(ShowDialog({ "Grrrrr!" }));
                    X(ShowDialog({ "I think OOP", "is really useful!" }));
                    X(MoveTo(m_pPlayer, 15, 15, 3.0f));
                    X(MoveTo(m_pPlayer, 10, 10, 3.0f));
                }
            }
        } else {
            if (m_bShowDialog) {
                if (GetKey(VK_SPACE).bReleased) {
                    m_bShowDialog = false;
                    m_script.CompleteCommand();
                }
            }
        }

        for (auto &object: m_vecDynamics) {
            float fNewObjectPosX = object->px + object->vx * fElapsedTime;
            float fNewObjectPosY = object->py + object->vy * fElapsedTime;

            if (object->vx > 10.0f) object->vx = 10.0f;
            if (object->vx < -10.0f) object->vx = -10.0f;
            if (object->vy > 100.0f) object->vy = 100.0f;
            if (object->vy < -100.0f) object->vy = -100.0f;

            if (object->vx <= 0) {
                if (m_pCurrentMap->GetSolid(fNewObjectPosX + 0.0f, object->py + 0.0f) || m_pCurrentMap->GetSolid(
                        fNewObjectPosX + 0.0f, object->py + 0.9f)) {
                    fNewObjectPosX = (int) fNewObjectPosX + 1;
                    object->vx = 0;
                }
            } else {
                if (m_pCurrentMap->GetSolid(fNewObjectPosX + 1.0f, object->py + 0.0f) || m_pCurrentMap->GetSolid(
                        fNewObjectPosX + 1.0f, object->py + 0.9f)) {
                    fNewObjectPosX = (int) fNewObjectPosX;
                    object->vx = 0;
                }
            }

            if (object->vy <= 0) {
                if (m_pCurrentMap->GetSolid(fNewObjectPosX + 0.0f, fNewObjectPosY) || m_pCurrentMap->GetSolid(
                        fNewObjectPosX + 0.9f, fNewObjectPosY)) {
                    fNewObjectPosY = (int) fNewObjectPosY + 1;
                    object->vy = 0;
                }
            } else {
                if (m_pCurrentMap->GetSolid(fNewObjectPosX + 0.0f, fNewObjectPosY + 1.0f) || m_pCurrentMap->GetSolid(
                        fNewObjectPosX + 0.9f, fNewObjectPosY + 1.0f)) {
                    fNewObjectPosY = (int) fNewObjectPosY;
                    object->vy = 0;
                }
            }

            object->px = fNewObjectPosX;
            object->py = fNewObjectPosY;

            object->Update(fElapsedTime);
        }
        fCameraPosX = m_pPlayer->px;
        fCameraPosY = m_pPlayer->py;

        // Draw Level
        int nTileWidth = 16; // 一个tile大小16 x 16
        int nTileHeight = 16;
        int nVisibleTilesX = ScreenWidth() / nTileWidth; // 一个屏幕nVisibleTilesX x nVisibleTilesY
        int nVisibleTilesY = ScreenHeight() / nTileHeight;

        float fOffsetX = fCameraPosX - (float) nVisibleTilesX / 2.0f;
        float fOffsetY = fCameraPosY - (float) nVisibleTilesY / 2.0f;

        if (fOffsetX < 0) fOffsetX = 0;
        if (fOffsetY < 0) fOffsetY = 0;
        if (fOffsetX > m_pCurrentMap->nWidth - nVisibleTilesX) fOffsetX = m_pCurrentMap->nWidth - nVisibleTilesX;
        if (fOffsetY > m_pCurrentMap->nHeight - nVisibleTilesY) fOffsetY = m_pCurrentMap->nHeight - nVisibleTilesY;

        float fTileOffsetX = (fOffsetX - (int) fOffsetX) * nTileWidth; // tile内的偏移，解决移动不平滑的问题
        float fTileOffsetY = (fOffsetY - (int) fOffsetY) * nTileHeight;

        // 逐项画出tile
        for (int x = -1; x < nVisibleTilesX + 1; x++) {
            for (int y = -1; y < nVisibleTilesY + 1; y++) {
                int idx = m_pCurrentMap->GetIndex(x + fOffsetX, y + fOffsetY);
                int sx = idx % 10;
                int sy = idx / 10;
                DrawPartialSprite(x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY, m_pCurrentMap->pSprite,
                                  sx * nTileWidth, sy * nTileHeight, nTileWidth, nTileHeight);
            }
        }

        for (auto &object: m_vecDynamics) object->DrawSelf(this, fOffsetX, fOffsetY);

        m_pPlayer->DrawSelf(this, fOffsetX, fOffsetY);
        if (m_bShowDialog)
            DisplayDialog(m_vecDialogToShow, 20, 20);
        return true;
    }

void RPG_Engine::ShowDialog(std::vector<std::string> vecLines) {
    m_vecDialogToShow = vecLines;
    m_bShowDialog = true;
}

void RPG_Engine::DisplayDialog(std::vector<std::string> vecText, int x, int y) {
    int nMaxLineLength = 0;
    int nLines = vecText.size();

    for (auto l: vecText) if (l.size() > nMaxLineLength) nMaxLineLength = l.size();

    // Draw Box
    Fill(x - 1, y - 1, x + nMaxLineLength * 8 + 1, y + nLines * 8 + 1, PIXEL_SOLID, FG_DARK_BLUE);
    DrawLine(x - 2, y - 2, x - 2, y + nLines * 8 + 1);
    DrawLine(x + nMaxLineLength * 8 + 1, y - 2, x + nMaxLineLength * 8 + 1, y + nLines * 8 + 1);
    DrawLine(x - 2, y - 2, x + nMaxLineLength * 8 + 1, y - 2);
    DrawLine(x - 2, y + nLines * 8 + 1, x + nMaxLineLength * 8 + 1, y + nLines * 8 + 1);

    for (int l = 0; l < vecText.size(); l++)
        DrawBigText(vecText[l], x, y + l * 8);
}