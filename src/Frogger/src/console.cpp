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
    protected:
        virtual bool OnUserCreate(){
            return true;
        }

        virtual bool OnUserUpdate(float fElapsedTime){
            fTimeSinceStart += fElapsedTime;
            for (auto lane : vecLanes){

            }
        }
    };

    void run_console_game() {
        OneLoneCoder_Frogger game;
        game.ConstructConsole(128, 80, 12, 12);
        game.Start();
    }
}

