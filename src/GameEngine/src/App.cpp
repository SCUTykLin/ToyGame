//
// Created by 15813 on 2026/8/24.
//
#include "App.h"
#include "olcConsoleGameEngine.h"

namespace olc3D {
    class olcEngine3D : public olcConsoleGameEngine {
    public:
        olcEngine3D() {
            m_sAppName = L"3D Engine By OlcEngine";
        }

        bool OnUserCreate() override {
            return true;
        }

        bool OnUserUpdate(float fElapsedTime) override {
            return true;
        }
    };

    void App::run() {
        olcEngine3D demo;
        if (demo.ConstructConsole(256, 240, 4, 4)) { demo.Start(); }
    }
}
