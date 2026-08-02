//
// Created by YangKai.Lin on 2026/8/3.
//

#include "game_sdl.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <vector>

namespace Tetris::SDL_Version {

    typedef struct Rect_Current {
        int nCurrentpiece = 0;
        int nCurrentRotation = 0;
        float nCurrentX = 0;
        float nCurrentY = 0;
        bool bRotateHold = false;
    }Tetris_Current;

    typedef struct Rect_Even {
        bool left = false;
        bool right = false;
        bool down = false;
        bool rotate = false;
    }Tetris_Even;

    typedef struct Rect {
        SDL_Rect rect;
        SDL_Color color;
    }Tetris_Rect;

    static SDL_Window* window = nullptr;
    static SDL_Renderer* renderer = nullptr;
    static bool isRunning = true;

    static int Rect_Size = 30;
    static constexpr int nFieldWidth = 12;
    static constexpr int nFieldHeight = 24;
    static unsigned char* pField = nullptr;

    static int sdl_nFieldWidth = nFieldWidth * Rect_Size;
    static int sdl_nFieldHeight = nFieldHeight * Rect_Size;
    static float speed = 0.2;

    std::string tetromino[7];
    SDL_Color tetrominoColor[9];

    void generateAssets() {
        tetromino[0].append("..X."
                            "..X."
                            "..X."
                            "..X.");
        tetromino[1].append("..X."
                            ".XX."
                            ".X.."
                            "....");
        tetromino[2].append("...."
                            ".XX."
                            ".XX."
                            "....");
        tetromino[3].append("..X."
                            ".XX."
                            ".X.."
                            "....");
        tetromino[4].append(".X.."
                            ".XX."
                            "..X."
                            "....");
        tetromino[5].append(".X.."
                            ".X.."
                            ".XX."
                            "....");
        tetromino[6].append("..X."
                            "..X."
                            ".XX."
                            "....");


        SDL_Color blue = {52, 152, 219, 255};

        SDL_Color green = {46, 204, 113, 255};

        SDL_Color yellow = {241, 196, 15, 255};

        SDL_Color orange = {230, 126, 15, 34};

        SDL_Color red = {192, 57, 43, 255};

        SDL_Color purple = {150, 0, 255, 255};

        SDL_Color pink = {255, 0, 102, 255};

        SDL_Color gray = {130, 130, 130, 255};

        tetrominoColor[1] = blue;
        tetrominoColor[2] = green;
        tetrominoColor[3] = yellow;
        tetrominoColor[4] = orange;
        tetrominoColor[5] = red;
        tetrominoColor[6] = purple;
        tetrominoColor[7] = pink;
        tetrominoColor[8] = gray;
    }

    bool game_sdl_init() {
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            return false;
        }
        window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, TETRIS_WIDTH, TETRIS_HEIGHT, SDL_WINDOW_SHOWN);
        if (window != nullptr) {
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); // Set the background color to white
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);
        }

        generateAssets();

        return true;
    }

    void gameEvent(SDL_Event& event, Tetris_Even& tetris_even) {
        tetris_even.left = false;
        tetris_even.right = false;
        tetris_even.down = false;
        tetris_even.rotate = false;

        switch (event.type) {
            case SDL_QUIT:
                isRunning = false;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    isRunning = false;
                }
                if (event.key.keysym.sym == SDLK_DOWN) {
                    tetris_even.down = true;
                }
                if (event.key.keysym.sym == SDLK_LEFT) {
                    tetris_even.left = true;
                }
                if (event.key.keysym.sym == SDLK_RIGHT) {
                    tetris_even.right = true;
                }
                 if (event.key.keysym.sym == SDLK_UP && event.key.repeat == 0) {
                    tetris_even.rotate = true;
                }
                break;
            default:
                break;
        }
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

    bool DoesPieceFit(int nTeromino, int nRotation, int nPosX, int nPosY) {
        for (int px = 0; px < 4; px++) {
            for (int py = 0; py < 4; py++) {
                int pi = Rotate(px, py, nRotation);
                int fi = (nPosY + py) * nFieldWidth + nPosX + px;

                if (nPosX + px >= 0 && nPosX + px < nFieldWidth) {
                    if (nPosY + py >= 0 && nPosY + py < nFieldHeight) {
                        if (tetromino[nTeromino][pi] != '.' && pField[fi] != 0) {
                            return false;
                        }
                    }
                }
            }
        }
        return true;
    }

    void gameLoop(Tetris_Even& tetris_even, Tetris_Current& tetris_current, std::vector<int>& vLine) {
        SDL_Delay(50);
        bool bGameOver = false;
        if (tetris_even.left && DoesPieceFit(tetris_current.nCurrentpiece, tetris_current.nCurrentRotation, tetris_current.nCurrentX - 1, tetris_current.nCurrentY))
            tetris_current.nCurrentX -= 1;
        if (tetris_even.right && DoesPieceFit(tetris_current.nCurrentpiece, tetris_current.nCurrentRotation, tetris_current.nCurrentX + 1, tetris_current.nCurrentY))
            tetris_current.nCurrentX += 1;
        if (tetris_even.down && DoesPieceFit(tetris_current.nCurrentpiece, tetris_current.nCurrentRotation, tetris_current.nCurrentX, tetris_current.nCurrentY + 1))
            tetris_current.nCurrentY += 1;

        if (!tetris_current.bRotateHold && tetris_even.rotate && DoesPieceFit(tetris_current.nCurrentpiece, tetris_current.nCurrentRotation + 1, tetris_current.nCurrentX, tetris_current.nCurrentY)) {
            tetris_current.nCurrentRotation += 1;
            tetris_current.bRotateHold = true;
        }else {
            tetris_current.bRotateHold = false;
        }

        if (DoesPieceFit(tetris_current.nCurrentpiece, tetris_current.nCurrentRotation, tetris_current.nCurrentX, tetris_current.nCurrentY + 1)) {
            tetris_current.nCurrentY += speed;
        }else {
            for (int px = 0; px < 4; ++px) {
                for (int py = 0; py < 4; ++py) {
                    if (tetromino[tetris_current.nCurrentpiece][Rotate(px, py, tetris_current.nCurrentRotation)] == 'X') {
                        pField[(int(tetris_current.nCurrentY) + py) * nFieldWidth + (int(tetris_current.nCurrentX) + px)] = tetris_current.nCurrentpiece + 1;
                    }
                }
            }

            for (int py = 0; py < 4; ++py) {
                if (tetris_current.nCurrentY + py < nFieldHeight - 1) {
                    bool bLine = true;
                    for (int px = 1; px < nFieldWidth-1; px++) {
                        bLine &= (pField[(int(tetris_current.nCurrentY) + py) * nFieldWidth + px]) != 0;
                    }
                    if (bLine) {
                        for (int px = 1; px < nFieldWidth-1; px++) {
                            pField[(int(tetris_current.nCurrentY) + py) * nFieldWidth + px] = 8;
                        }
                        vLine.push_back(int(tetris_current.nCurrentY) + py);
                    }
                }
            }
            // Choose next piece
            tetris_current.nCurrentX = nFieldWidth / 2;
            tetris_current.nCurrentY = 0;
            tetris_current.nCurrentRotation = 0;
            tetris_current.nCurrentpiece = rand() % 7;

            // If piece does not fit
            bGameOver = !DoesPieceFit(tetris_current.nCurrentpiece, tetris_current.nCurrentRotation, tetris_current.nCurrentX, tetris_current.nCurrentY);
        }
    }

    void gameRender(Tetris_Current& tetris_current, std::vector<int>& vLine) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render Environment
        for (int x = 0; x < nFieldWidth; ++x) { // Draw Field
            for (int y = 0; y < nFieldHeight; ++y) {
                if (pField[y * nFieldWidth + x] == 9) {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_Rect bg = {x * Rect_Size, y * Rect_Size, Rect_Size, Rect_Size};
                    SDL_RenderFillRect(renderer, &bg);
                }else if (pField[y * nFieldWidth + x] != 0){
                    SDL_SetRenderDrawColor(renderer,
                        tetrominoColor[pField[y * nFieldWidth + x]].r,
                        tetrominoColor[pField[y * nFieldWidth + x]].g,
                        tetrominoColor[pField[y * nFieldWidth + x]].b,
                        tetrominoColor[pField[y * nFieldWidth + x]].a);
                    SDL_Rect bg = {x * Rect_Size, y * Rect_Size, Rect_Size, Rect_Size};
                    SDL_RenderFillRect(renderer, &bg);
                }
            }
        }
        // Render Object
        for (int px = 0; px < 4; px++) {
            for (int py = 0; py < 4; py++) {
                if (tetromino[tetris_current.nCurrentpiece][Rotate(px, py, tetris_current.nCurrentRotation)] == 'X'){
                    SDL_SetRenderDrawColor(renderer,
                        tetrominoColor[tetris_current.nCurrentpiece + 1].r,
                        tetrominoColor[tetris_current.nCurrentpiece + 1].g,
                        tetrominoColor[tetris_current.nCurrentpiece + 1].b,
                        tetrominoColor[tetris_current.nCurrentpiece + 1].a);
                    SDL_Rect rect = {(px + int(tetris_current.nCurrentX)) * Rect_Size, (py + int(tetris_current.nCurrentY)) * Rect_Size, Rect_Size, Rect_Size};
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }

        // Delete
        if (!vLine.empty()) {
            for (auto &v : vLine)
                for (int px = 1; px < nFieldWidth - 1; px++){
                    for (int py = v; py > 0; py--)
                        pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
                    pField[px] = 0;
                }
            vLine.clear();
        }

        SDL_RenderPresent(renderer);
    }

    void gameExit() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        window = nullptr;
        renderer = nullptr;
        SDL_Quit();
    }

    void mainLoop(){
        if (!game_sdl_init()) {
            std::cout << "SDL Inited in failure. " << SDL_GetError() << std::endl;
            return;
        }

        pField = new unsigned char[nFieldWidth * nFieldHeight];
        for (int x = 0; x < nFieldWidth; x++) {
            for (int y = 0; y < nFieldHeight; y++) {
                pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;
            }
        }

        SDL_Event event;
        Tetris_Even tetris_even;
        Tetris_Current tetris_current;

        tetris_current.nCurrentRotation = 0;
        tetris_current.nCurrentX = nFieldWidth / 2;
        tetris_current.nCurrentY = 0;
        tetris_current.nCurrentpiece = 1;
        std::vector<int> vLine;
        while (isRunning) {
            while (SDL_PollEvent(&event)) {
                gameEvent(event, tetris_even);
            }
            gameLoop(tetris_even, tetris_current, vLine);

            gameRender(tetris_current, vLine);
        }
        gameExit();
    }

}
