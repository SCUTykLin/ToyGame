//
// Created by YangKai.Lin on 2026/7/26.
//

#include "game.h"
#include <iostream>
#include <windows.h>
#include <thread>
#include <vector>

namespace Tetris {
    std::wstring tetromino[7];
    constexpr int nFieldWidth = 12;
    constexpr int nFieldHeight = 18;
    constexpr int nScreenWidth = 80;
    constexpr int nScreenHeight = 30;
    unsigned char* pField = nullptr;

    void generateAssets(std::wstring* _tetromino) {
        _tetromino[0].append(L"..X...X...X...X.");
        _tetromino[1].append(L"..X..XX..X......");
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
     * pi表示旋转之后，对应在4x4格子里面的index，也就是旋转之后(px, py)是什么。
     * fi表示(px, py)在全局下的索引。
     */
    bool DoesPieceFit(int nTeromino, int nRotation, int nPosX, int nPosY) {
        for (int px = 0; px < 4; px++) {
            for (int py = 0; py < 4; py++) {
                int pi = Rotate(px, py, nRotation);
                int fi = (nPosY + py) * nFieldWidth + nPosX + px;

                if (nPosX + px >= 0 && nPosX + px < nFieldWidth) {
                    if (nPosY + py >= 0 && nPosY + py < nFieldHeight) {
                        if (tetromino[nTeromino][pi] != L'.' && pField[fi] != 0) {
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

        int nCurrentpiece = 1;
        int nCurrentRotation = 0;
        int nCurrentX = nFieldWidth / 2;
        int nCurrentY = 0;
        bool bKey[4];
        bool bRotateHold = false;

        int nSpeed = 20;
        int nSpeedCounter = 0;
        bool bForceDown = false;
        std::vector<int> vLine;

        while (!bGameOver) {
            // Game Timing
            Sleep(50);
            nSpeedCounter++;
            bForceDown = (nSpeedCounter == nSpeed);

            // Input
            static constexpr BYTE vkTable[] = {VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN};
            for (int k = 0; k < 4; k++) {
                SHORT st = GetAsyncKeyState(vkTable[k]);
                bKey[k] = (st & 0x8000) != 0;
            }

            // Game Logic
            nCurrentX -= (bKey[0] && DoesPieceFit(nCurrentpiece, nCurrentRotation, nCurrentX-1, nCurrentY)) ? 1 : 0;
            nCurrentX += (bKey[1] && DoesPieceFit(nCurrentpiece, nCurrentRotation, nCurrentX+1, nCurrentY)) ? 1 : 0;
            nCurrentY += (bKey[3] && DoesPieceFit(nCurrentpiece, nCurrentRotation, nCurrentX, nCurrentY+1)) ? 1 : 0;

            if (bKey[2]) {
                nCurrentRotation += (!bRotateHold && DoesPieceFit(nCurrentpiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;
                bRotateHold = true;
            }else {
                bRotateHold = false;
            }

            if (bForceDown) {
                if (DoesPieceFit(nCurrentpiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) {
                    ++nCurrentY;
                }else{
                    // Lock the currenr piece in the field;
                    for (int px = 0; px < 4; ++px) {
                        for (int py = 0; py < 4; ++py) {
                            if (tetromino[nCurrentpiece][Rotate(px, py, nCurrentRotation)] == L'X') {
                                pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentpiece + 1;
                            }
                        }
                    }

                    // check any lines
                    for (int py = 0; py < 4; ++py) {
                        if (nCurrentY + py < nFieldHeight - 1) {
                            bool bLine = true;
                            for (int px = 1; px < nFieldWidth-1; px++) {
                                bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;
                            }
                            if (bLine) {
                                for (int px = 1; px < nFieldWidth-1; px++) {
                                    pField[(nCurrentY + py) * nFieldWidth + px] = 8;
                                }
                                vLine.push_back(nCurrentY + py);
                            }
                        }
                    }


                    // choose next piece
                    nCurrentX = nFieldWidth / 2;
                    nCurrentY = 0;
                    nCurrentRotation = 0;
                    nCurrentpiece = rand() % 7;

                    // if piece does not fit
                    bGameOver = !DoesPieceFit(nCurrentpiece, nCurrentRotation, nCurrentX, nCurrentY);
                }
                nSpeedCounter = 0;
            }

            // Render Output
            for (int x = 0; x < nFieldWidth; ++x) { // Draw Field
                for (int y = 0; y < nFieldHeight; ++y) {
                    screen[(y + 2) * nScreenWidth + (x + 2)] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]];
                }
            }

            // Draw Current Piece
            for (int px = 0; px < 4; px++){
                for (int py = 0; py < 4; py++){
                    if (tetromino[nCurrentpiece][Rotate(px, py, nCurrentRotation)] == L'X'){
                        screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentpiece + 65;
                    }
                }
            }

            if (!vLine.empty()) {
                WriteConsoleOutputCharacterW(hConsole, screen, nScreenWidth * nScreenHeight, {0, 0}, &dwBytesWritten);
                Sleep(400);

                for (auto &v : vLine)
                    for (int px = 1; px < nFieldWidth - 1; px++){
                        for (int py = v; py > 0; py--)
                            pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
                        pField[px] = 0;
                    }
                vLine.clear();
            }
            WriteConsoleOutputCharacterW(hConsole, screen, nScreenWidth * nScreenHeight, {0, 0}, &dwBytesWritten);
        }
    }
}
