//
// Created by YangKai.Lin on 2026/7/26.
//

#include "game.h"
#include <iostream>
#include <windows.h>

namespace Tetris {
    std::wstring tetromino[7];
    constexpr int nFieldWidth = 12;
    constexpr int nFieldHeight = 18;
    constexpr int nScreenWidth = 80;
    constexpr int nScreenHeight = 30;
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
            case 0: return py * 4 + px;         // 0 degrees
            case 1: return 12 + py - (px * 4);  // 90 degrees
            case 2: return  15 - (py * 4) - px; // 180 degrees
            case 3: return 3 - py + (px *4);    // 270 degrees
            default: return -1;
        }
    }

    void setConsoleSize(HANDLE& hConsole, short width, short height) {
        COORD bufferSize = { (SHORT)width, (SHORT)height };
        SetConsoleScreenBufferSize(hConsole, bufferSize);

        SMALL_RECT windowRect = { 0, 0, (SHORT)(width - 1), (SHORT)(height - 1) };
        SetConsoleWindowInfo(hConsole, TRUE, &windowRect);
    }

    /*
     * 每一个方块只是在4x4的格子里面，所以只需要判断当前4x4格子是否能旋转就行。
     * pi表示旋转之后，对应在4x4格子里面的index，也就是旋转之后(px, py)是什么
     * fi表示(px, py)在全局下的索引
     */
    bool DoesPieceFit(int nTeromino, int nRotation, int nPosX, int nPosY) {
        for (int px = 0; px < 4; px++) {
            for (int py = 0; py < 4; py++) {
                int pi = Rotate(px, py, nRotation);
                int fi = (nPosX + py) * nFieldWidth + nPosX + px;

                if (nPosX + px >= 0 && nPosX + px < nFieldWidth) {
                    if (nPosY + py >= 0 && nPosY + py < nFieldHeight) {
                        if (tetromino[nTeromino][pi] == L'X' && pField[fi] != 0) {
                            return false;
                        }
                    }
                }
            }
        }
        return true;
    }

    void mainLoop() {
        generateAssets(tetromino);
        pField = new unsigned char[nFieldWidth * nFieldHeight];
        for (int x = 0; x < nFieldWidth; x++) {
            for (int y = 0; y < nFieldHeight; y++) {
                pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;
            }
        }
        wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
        for (int i = 0; i < nScreenWidth * nScreenHeight; i++) {
            screen[i] = L' ';
        }
        HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, nullptr, CONSOLE_TEXTMODE_BUFFER, nullptr);
        setConsoleSize(hConsole, static_cast<short>(nScreenWidth), static_cast<short>(nScreenHeight));
        SetConsoleActiveScreenBuffer(hConsole);
        DWORD dwBytesWritten = 0;

        bool bGameOver = false;
        while (!bGameOver) {
            // Game Timing

            // Input

            // Game Logic

            // Render Output

            for (int x = 0; x < nFieldWidth; ++x) { // Draw Field
                for (int y = 0; y < nFieldHeight; ++y) {
                    screen[(y + 2) * nScreenWidth + (x + 2)] = L" ABCDEFG=█"[pField[y * nFieldWidth + x]];
                }
            }
            WriteConsoleOutputCharacterW(hConsole, screen, nScreenWidth * nScreenHeight, {0, 0}, &dwBytesWritten);
        }
    }
}