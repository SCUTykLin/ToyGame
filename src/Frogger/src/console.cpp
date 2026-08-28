//
// Created by 15813 on 2026/8/25.
//

#include "console.h"
#include "olcConsoleGameEngine.h"

namespace Frogger::Console{

    class OneLoneCoder_Frogger : public olcConsoleGameEngine{
    public:
        OneLoneCoder_Frogger(){
            m_sAppName = L"Frogger";
        }
    private:
        std::vector<std::pair<float, std::wstring>> vecLanes = {
                {0.0f,  L"wwwhhwwwhhwwwhhwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww"}, // 64 elements per lane
                {-3.0f, L",,,jllk,,jllllk,,,,,,,jllk,,,,,jk,,,jlllk,,,,jllllk,,,,jlllk,,,,"},
                {3.0f,  L",,,,jllk,,,,,jllk,,,,jllk,,,,,,,,,jllk,,,,,jk,,,,,,jllllk,,,,,,,"},
                {2.0f,  L",,jlk,,,,,jlk,,,,,jk,,,,,jlk,,,jlk,,,,jk,,,,jllk,,,,jk,,,,,,jk,,"},
                {0.0f,  L"pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp"},
                {-3.0f, L"....asdf.......asdf....asdf..........asdf........asdf....asdf..."},
                { 3.0f, L".....ty..ty....ty....ty.....ty........ty..ty.ty......ty.......ty"},
                {-4.0f, L"..zx.....zx.........zx..zx........zx...zx...zx....zx...zx...zx.."},
                {2.0f,  L"..ty.....ty.......ty.....ty......ty..ty.ty.......ty....ty......."},
                {0.0f,  L"pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp"}
        };
        float fTimeSinceStart = 0.0f;
        int nCellSize = 8;

        bool *bufDanger = nullptr;

        olcSprite *spriteBus = nullptr;
        olcSprite *spriteLog = nullptr;
        olcSprite *spriteWater = nullptr;
        olcSprite *spriteFrog = nullptr;
        olcSprite *spritePavement = nullptr;
        olcSprite *spriteCar1 = nullptr;
        olcSprite *spriteCar2 = nullptr;
        olcSprite *spriteWall = nullptr;
        olcSprite *spriteHome = nullptr;

    protected:
        virtual bool OnUserCreate(){
            spriteBus = new olcSprite(L"src/Frogger/assets/bus.spr");
            spriteLog = new olcSprite(L"src/Frogger/assets/log.spr");
            spriteWater = new olcSprite(L"src/Frogger/assets/water.spr");
            spriteFrog = new olcSprite(L"src/Frogger/assets/frog.spr");
            spritePavement = new olcSprite(L"src/Frogger/assets/pavement.spr");
            spriteWall = new olcSprite(L"src/Frogger/assets/wall.spr");
            spriteHome = new olcSprite(L"src/Frogger/assets/home.spr");
            spriteCar1 = new olcSprite(L"src/Frogger/assets/car1.spr");
            spriteCar2 = new olcSprite(L"src/Frogger/assets/car2.spr");
            return true;
        }

        virtual bool OnUserUpdate(float fElapsedTime){
            fTimeSinceStart += fElapsedTime;
            int x = -1, y = 0;
            for (auto lane : vecLanes){
                // Find offset
                int nStartPos = (int)(fTimeSinceStart * lane.first) % 64;
                int nCellOffset = (int)((float)nCellSize * fTimeSinceStart * lane.first) % nCellSize; // 在格子内移动了多少
                for (int i = 0; i < ScreenWidth() / nCellSize + 2; ++i){
                    wchar_t graphic = lane.second[(nStartPos + i + 64) % 64];
                    // Fill((x + i) * nCellSize - nCellOffset, y * nCellSize,
                    //      (x + i + 1) * nCellSize - nCellOffset, (y + 1) * nCellSize, graphic);
                    switch (graphic) {
                        case L'z': DrawPartialSprite((x + i) * nCellSize - nCellOffset, y * nCellSize, spriteCar2, 0, 0, 8, 8); break;
                        case L'x': DrawPartialSprite((x + i) * nCellSize - nCellOffset, y * nCellSize, spriteCar2, 8, 0, 8, 8); break;
                    }
                }
                y++;
            }
            return true;
        }
    };

    void run_console_game() {
        OneLoneCoder_Frogger game;
        game.ConstructConsole(132, 96, 8, 8);
        game.Start();
    }
}

