//
// Created by YangKai.Lin on 2026/8/30.
//

#include "App.h"
#include <iostream>
#include <string>
#include "olcConsoleGameEngine.h"
#include <cwchar>

class OneLoneCoder_Platformer : public olcConsoleGameEngine {
public:
    OneLoneCoder_Platformer() {
        m_sAppName = L"Tile Based Platform Game";
    }

private:
    std::wstring sLevel;
    int nLevelWidth;
    int nLevelHeight;

    float fPlayerPosX = 1.0f;
    float fPlayerPosY = 1.0f;

    float fPlayerVelX = 0.0f;
    float fPlayerVelY = 0.0f;

    bool bPlayerOnGround = false;

    float fCameraPosX = 0.0f;
    float fCameraPosY = 0.0f;

    olcSprite *spriteTiles = nullptr;
    olcSprite *spriteMan = nullptr;

    int nDirModX = 0;
    int nDirModY = 0;

protected:
    virtual bool OnUserCreate() {
        nLevelWidth = 64;
        nLevelHeight = 16;
        sLevel += L"................................................................";
        sLevel += L"................................................................";
        sLevel += L".......ooooo....................................................";
        sLevel += L"........ooo.....................................................";
        sLevel += L".......................########.................................";
        sLevel += L".....BB?BBBB?BB.......###..............#.#......................";
        sLevel += L"....................###................#.#......................";
        sLevel += L"...................####.........................................";
        sLevel += L"GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG.##############.....########";
        sLevel += L"...................................#.#...............###........";
        sLevel += L"........................############.#............###...........";
        sLevel += L"........................#............#.........###..............";
        sLevel += L"........................#.############......###.................";
        sLevel += L"........................#................###....................";
        sLevel += L"........................#################.......................";
        sLevel += L"................................................................";

        spriteTiles = new olcSprite(L"src/TileBasedPlatform/assets/leveljario.spr");
        spriteMan = new olcSprite(L"src/TileBasedPlatform/assets/minijario.spr");
        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) {
        // Utility Lambdas
        auto GetTile = [&](int x, int y) {
            if (x >= 0 && x < nLevelWidth && y >= 0 && y < nLevelHeight) {
                return sLevel[y * nLevelWidth + x];
            }
            return L' ';
        };

        auto SetTile = [&](int x, int y, wchar_t c) {
            if (x >= 0 && x < nLevelWidth && y >= 0 && y < nLevelHeight) {
                sLevel[y * nLevelWidth + x] = c;
            }
        };

        constexpr float speed = 10.0f;

        // Handle Input
        if (IsFocused()) {
            if (GetKey(VK_UP).bHeld) {
                fPlayerVelY = -6.0f;
            }
            if (GetKey(VK_DOWN).bHeld) {
                fPlayerVelY = 6.0f;
            }
            if (GetKey(VK_LEFT).bHeld) {
                fPlayerVelX += (bPlayerOnGround ? -speed : -4.0f) * fElapsedTime;
                nDirModY = 1;
            }
            if (GetKey(VK_RIGHT).bHeld) {
                fPlayerVelX += (bPlayerOnGround ? speed : 4.0f) * fElapsedTime;
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

        if (GetTile(fNewPlayerPosX + 0.0f, fNewPlayerPosY + 0.0f) == L'o')
            SetTile(fNewPlayerPosX + 0.0f, fNewPlayerPosY + 0.0f, L'.');

        if (GetTile(fNewPlayerPosX + 0.0f, fNewPlayerPosY + 1.0f) == L'o')
            SetTile(fNewPlayerPosX + 0.0f, fNewPlayerPosY + 1.0f, L'.');

        if (GetTile(fNewPlayerPosX + 1.0f, fNewPlayerPosY + 0.0f) == L'o')
            SetTile(fNewPlayerPosX + 1.0f, fNewPlayerPosY + 0.0f, L'.');

        if (GetTile(fNewPlayerPosX + 1.0f, fNewPlayerPosY + 1.0f) == L'o')
            SetTile(fNewPlayerPosX + 1.0f, fNewPlayerPosY + 1.0f, L'.');

        if (fPlayerVelX > 10.0f) fPlayerVelX = 10.0f;
        if (fPlayerVelX < -10.0f) fPlayerVelX = -10.0f;
        if (fPlayerVelY > 100.0f) fPlayerVelY = 100.0f;
        if (fPlayerVelY < -100.0f) fPlayerVelY = -100.0f;

        if (fPlayerVelX <= 0) {
            if (GetTile(fNewPlayerPosX + 0.0f, fPlayerPosY + 0.0f) != L'.' || GetTile(
                    fNewPlayerPosX + 0.0f, fPlayerPosY + 0.9f) != L'.') {
                fNewPlayerPosX = (int) fNewPlayerPosX + 1;
                fPlayerVelX = 0;
            }
        } else {
            if (GetTile(fNewPlayerPosX + 1.0f, fPlayerPosY + 0.0f) != L'.' || GetTile(
                    fNewPlayerPosX + 1.0f, fPlayerPosY + 0.9f) != L'.') {
                fNewPlayerPosX = (int) fNewPlayerPosX;
                fPlayerVelX = 0;
            }
        }

        bPlayerOnGround = true;
        if (fPlayerVelY <= 0) {
            if (GetTile(fNewPlayerPosX + 0.0f, fNewPlayerPosY) != L'.' || GetTile(fNewPlayerPosX + 0.9f, fNewPlayerPosY)
                != L'.') {
                fNewPlayerPosY = (int) fNewPlayerPosY + 1;
                fPlayerVelY = 0;
            }
        } else {
            if (GetTile(fNewPlayerPosX + 0.0f, fNewPlayerPosY + 1.0f) != L'.' || GetTile(
                    fNewPlayerPosX + 0.9f, fNewPlayerPosY + 1.0f) != L'.') {
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
        if (fOffsetX > nLevelWidth - nVisibleTilesX) fOffsetX = nLevelWidth - nVisibleTilesX;
        if (fOffsetY > nLevelHeight - nVisibleTilesY) fOffsetY = nLevelHeight - nVisibleTilesY;

        float fTileOffsetX = (fOffsetX - (int) fOffsetX) * nTileWidth; // tile内的偏移，解决移动不平滑的问题
        float fTileOffsetY = (fOffsetY - (int) fOffsetY) * nTileHeight;

        // 逐项画出tile
        for (int x = -1; x < nVisibleTilesX + 1; x++) {
            for (int y = -1; y < nVisibleTilesY + 1; y++) {
                wchar_t sTileID = GetTile(x + fOffsetX, y + fOffsetY);
                switch (sTileID) {
                    case L'.': // Sky
                        Fill(x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY,
                             (x + 1) * nTileWidth - fTileOffsetX, (y + 1) * nTileHeight - fTileOffsetY, PIXEL_SOLID,
                             FG_CYAN);
                        break;
                    case L'#': // Solid Block
                        //Fill(x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY, (x + 1) * nTileWidth - fTileOffsetX, (y + 1) * nTileHeight - fTileOffsetY, PIXEL_SOLID, FG_RED);
                        DrawPartialSprite(x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY, spriteTiles,
                                          2 * nTileWidth, 0 * nTileHeight, nTileWidth, nTileHeight);
                        break;
                    case L'G': // Ground Block
                        DrawPartialSprite(x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY, spriteTiles,
                                          0 * nTileWidth, 0 * nTileHeight, nTileWidth, nTileHeight);
                        break;
                    case L'B': // Brick Block
                        DrawPartialSprite(x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY, spriteTiles,
                                          0 * nTileWidth, 1 * nTileHeight, nTileWidth, nTileHeight);
                        break;
                    case L'?': // Question Block
                        DrawPartialSprite(x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY, spriteTiles,
                                          1 * nTileWidth, 1 * nTileHeight, nTileWidth, nTileHeight);
                        break;
                    case L'o': // Coin
                        Fill(x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY,
                             (x + 1) * nTileWidth - fTileOffsetX, (y + 1) * nTileHeight - fTileOffsetY, PIXEL_SOLID,
                             FG_CYAN);
                        DrawPartialSprite(x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY, spriteTiles,
                                          3 * nTileWidth, 0 * nTileHeight, nTileWidth, nTileHeight);
                        break;
                    default:
                        Fill(x * nTileWidth - fTileOffsetX, y * nTileHeight - fTileOffsetY,
                             (x + 1) * nTileWidth - fTileOffsetX, (y + 1) * nTileHeight - fTileOffsetY, PIXEL_SOLID,
                             FG_BLACK);
                        break;
                }
            }
        }
        DrawPartialSprite((fPlayerPosX - fOffsetX) * nTileWidth, (fPlayerPosY - fOffsetY) * nTileWidth, spriteMan, nDirModX * nTileWidth, nDirModY * nTileHeight, nTileWidth, nTileHeight);
        return true;
    }
};

namespace TileBased {
    void App::run() {
        OneLoneCoder_Platformer game;
        if (game.ConstructConsole(256, 240, 4, 4)) {
            game.Start();
        }
    }
}
