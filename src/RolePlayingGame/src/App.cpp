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
        m_pPlayer->px = 5.0f;
        m_pPlayer->py = 5.0f;

        spriteTiles = new olcSprite(L"src/TileBasedPlatform/assets/leveljario.spr");
        spriteMan = new olcSprite(L"src/TileBasedPlatform/assets/minijario.spr");
        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) {
        m_pPlayer->vx = 0.0f;
        m_pPlayer->vy = 0.0f;
        // Handle Input
        if (IsFocused()) {
            if (GetKey(VK_UP).bHeld){
                m_pPlayer->vy = -4.0f;
            }

            if (GetKey(VK_DOWN).bHeld){
                m_pPlayer->vy = 4.0f;
            }

            if (GetKey(VK_LEFT).bHeld){
                m_pPlayer->vx = -4.0f;
            }

            if (GetKey(VK_RIGHT).bHeld){
                m_pPlayer->vx = 4.0f;
            }
        }

        RoleGame::cDynamic* object = m_pPlayer;
        float fNewObjectPosX = object->px + object->vx * fElapsedTime;
        float fNewObjectPosY = object->py + object->vy * fElapsedTime;

        if (object->vx > 10.0f) object->vx = 10.0f;
        if (object->vx < -10.0f) object->vx = -10.0f;
        if (object->vy > 100.0f) object->vy = 100.0f;
        if (object->vy < -100.0f) object->vy = -100.0f;

        if (object->vx <= 0) {
            if (m_pCurrentMap->GetSolid(fNewObjectPosX + 0.0f, object->py + 0.0f) || m_pCurrentMap->GetSolid(fNewObjectPosX + 0.0f, object->py + 0.9f)) {
                fNewObjectPosX = (int) fNewObjectPosX + 1;
                object->vx = 0;
            }
        } else {
            if (m_pCurrentMap->GetSolid(fNewObjectPosX + 1.0f, object->py + 0.0f) || m_pCurrentMap->GetSolid(fNewObjectPosX + 1.0f, object->py + 0.9f)) {
                fNewObjectPosX = (int) fNewObjectPosX;
                object->vx = 0;
            }
        }

        bPlayerOnGround = true;
        if (object->vy <= 0) {
            if (m_pCurrentMap->GetSolid(fNewObjectPosX + 0.0f, fNewObjectPosY) || m_pCurrentMap->GetSolid(fNewObjectPosX + 0.9f, fNewObjectPosY)) {
                fNewObjectPosY = (int) fNewObjectPosY + 1;
                object->vy = 0;
            }
        } else {
            if (m_pCurrentMap->GetSolid(fNewObjectPosX + 0.0f, fNewObjectPosY + 1.0f) || m_pCurrentMap->GetSolid(fNewObjectPosX + 0.9f, fNewObjectPosY + 1.0f)) {
                fNewObjectPosY = (int) fNewObjectPosY;
                object->vy = 0;
                bPlayerOnGround = true;
                nDirModX = 0;
            }
        }

        object->px = fNewObjectPosX;
        object->py = fNewObjectPosY;

        object->Update(fElapsedTime);

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
                DrawPartialSprite(x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY, m_pCurrentMap->pSprite, sx * nTileWidth, sy * nTileHeight, nTileWidth, nTileHeight);
            }
        }
        object->DrawSelf(this, fOffsetX, fOffsetY);

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
