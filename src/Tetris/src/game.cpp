//
// Created by YangKai.Lin on 2026/7/26.
//

#include "game.h"
#include <iostream>

namespace Tetris {
    std::wstring tetromino[7];
    constexpr int nFieldWidth = 12;
    constexpr int nFieldHeight = 18;
    unsigned char* pField = nullptr;
    void generateAssets(std::wstring* _tetromino) {
        _tetromino[0].append(L"..X...X...X...X.");
        _tetromino[1].append(L"..X..XX...X.....");
        _tetromino[2].append(L".....XX..XX.....");
        _tetromino[3].append(L"..X..XX..X......");
        _tetromino[4].append(L".X...XX...X.....");
        _tetromino[5].append(L".X...X...XX.....");
        _tetromino[6].append(L"..X...X..XX.....");
    }

    int Rotate(int px, int py, int r) {
        switch (r % 4) {
            case 0: return py * 4 + px;        // 0 degrees
            case 1: return 12 + py - (px * 4); // 90 degrees
            case 2: return  15 - (py * 4) - px; // 180 degrees
            case 3: return 3 - py + (px *4); // 270 degrees
        }
    }

    void mainLoop() {
        generateAssets(tetromino);
    }
}