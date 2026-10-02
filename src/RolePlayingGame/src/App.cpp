//
// Created by YangKai.Lin on 2026/8/30.
//

#include "App.h"
#include <iostream>
#include <string>
#include "olcConsoleGameEngine.h"
#include "RPG_Maps.h"
#include "RPG_Assets.h"
#include "RPG_Dynamic.h"

class OneLoneCoder_RPG : public olcConsoleGameEngine {
public:
    OneLoneCoder_RPG() {
        m_sAppName = L"Role Play Game";
    }

private:
    RoleGame::cMap* m_pCurrentMap = nullptr;

    RoleGame::cDynamic* m_pPlayer = nullptr;

    bool bPlayerOnGround = false;

    float fCameraPosX = 0.0f;
    float fCameraPosY = 0.0f;

    olcSprite *spriteTiles = nullptr;
    olcSprite *spriteMan = nullptr;
    olcSprite *m_sprFont = nullptr;

    int nDirModX = 0;
    int nDirModY = 0;

    void DrawBigText(const std::string& sText, int x, int y){
        int i = 0;
        for (auto c : sText)
        {
            int sx = ((c - 32) % 16) * 8;
            int sy = ((c - 32) / 16) * 8;
            DrawPartialSprite(x + i * 8, y, m_sprFont, sx, sy, 8, 8);
            i++;
        }
    }

protected:
    virtual bool OnUserCreate() {
        RoleGame::RPG_Assets::get().LoadSprites();
        m_sprFont = RoleGame::RPG_Assets::get().GetSprite("font");
        m_pCurrentMap = new RoleGame::cMap_Village1();
        m_pPlayer = new RoleGame::cDynamic_Creature("player", RoleGame::RPG_Assets::get().GetSprite("player"));
        spriteTiles = new olcSprite(L"src/TileBasedPlatform/assets/leveljario.spr");
        spriteMan = new olcSprite(L"src/TileBasedPlatform/assets/minijario.spr");
        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) {
        fPlayerVelX = 0.0f;
        fPlayerVelY = 0.0f;

        // Handle Input
        if (IsFocused()) {
            if (GetKey(VK_UP).bHeld) {
                fPlayerVelY = -6.0f;
            }
            if (GetKey(VK_DOWN).bHeld) {
                fPlayerVelY = 6.0f;
            }
            if (GetKey(VK_LEFT).bHeld) {
                fPlayerVelX += (bPlayerOnGround ? -25.0f : -15.0f) * fElapsedTime;
                nDirModY = 1;
            }
            if (GetKey(VK_RIGHT).bHeld) {
                fPlayerVelX += (bPlayerOnGround ? 25 : 15.0f) * fElapsedTime;
                nDirModY = 0;
            }
            if (GetKey(VK_SPACE).bPressed) {
                if (fPlayerVelY == 0) {
                    fPlayerVelY = -12.0f;
                    nDirModX = 1;
                }
            }
        }
        fPlayerVelY += 20.0f * fElapsedTime;
        if (bPlayerOnGround) {
            fPlayerVelX += -3.0f * fPlayerVelX * fElapsedTime;
            if (fabs(fPlayerVelX) < 0.01f) fPlayerVelX = 0.0f;
        }

        float fNewPlayerPosX = fPlayerPosX + fPlayerVelX * fElapsedTime;
        float fNewPlayerPosY = fPlayerPosY + fPlayerVelY * fElapsedTime;

        if (fPlayerVelX > 10.0f) fPlayerVelX = 10.0f;
        if (fPlayerVelX < -10.0f) fPlayerVelX = -10.0f;
        if (fPlayerVelY > 100.0f) fPlayerVelY = 100.0f;
        if (fPlayerVelY < -100.0f) fPlayerVelY = -100.0f;

        if (fPlayerVelX <= 0) {
            if (m_pCurrentMap->GetSolid(fNewPlayerPosX + 0.0f, fPlayerPosY + 0.0f) || m_pCurrentMap->GetSolid(fNewPlayerPosX + 0.0f, fPlayerPosY + 0.9f)) {
                fNewPlayerPosX = (int) fNewPlayerPosX + 1;
                fPlayerVelX = 0;
            }
        } else {
            if (m_pCurrentMap->GetSolid(fNewPlayerPosX + 1.0f, fPlayerPosY + 0.0f) || m_pCurrentMap->GetSolid(fNewPlayerPosX + 1.0f, fPlayerPosY + 0.9f)) {
                fNewPlayerPosX = (int) fNewPlayerPosX;
                fPlayerVelX = 0;
            }
        }

        bPlayerOnGround = true;
        if (fPlayerVelY <= 0) {
            if (m_pCurrentMap->GetSolid(fNewPlayerPosX + 0.0f, fNewPlayerPosY) || m_pCurrentMap->GetSolid(fNewPlayerPosX + 0.9f, fNewPlayerPosY)) {
                fNewPlayerPosY = (int) fNewPlayerPosY + 1;
                fPlayerVelY = 0;
            }
        } else {
            if (m_pCurrentMap->GetSolid(fNewPlayerPosX + 0.0f, fNewPlayerPosY + 1.0f) || m_pCurrentMap->GetSolid(fNewPlayerPosX + 0.9f, fNewPlayerPosY + 1.0f)) {
                fNewPlayerPosY = (int) fNewPlayerPosY;
                fPlayerVelY = 0;
                bPlayerOnGround = true;
                nDirModX = 0;
            }
        }

        fPlayerPosX = fNewPlayerPosX;
        fPlayerPosY = fNewPlayerPosY;

        fCameraPosX = fPlayerPosX;
        fCameraPosY = fPlayerPosY;

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
                DrawPartialSprite(x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY, m_pCurrentMap->pSprite, sx * nTileWidth, sy * nTileHeight, nTileWidth, nTileHeight);
            }
        }
        DrawPartialSprite((fPlayerPosX - fOffsetX) * nTileWidth, (fPlayerPosY - fOffsetY) * nTileWidth, spriteMan, nDirModX * nTileWidth, nDirModY * nTileHeight, nTileWidth, nTileHeight);

        DrawBigText("Hello Everybody", 30, 30);

        return true;
    }
};

namespace RoleGame {
    void App::run() {
        OneLoneCoder_RPG game;
        if (game.ConstructConsole(256, 240, 4, 4)) {
            game.Start();
        }
    }
}
