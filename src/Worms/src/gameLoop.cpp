//
// Created by YangKai.Lin on 2026/8/30.
//
#include <iostream>
#include <string>
#include <algorithm>
#include "olcConsoleGameEngine.h"

namespace Worms {

    class OneLoneCoder_Worms : public olcConsoleGameEngine {
    public:
        OneLoneCoder_Worms() {
            m_sAppName = L"Worms";
        }
    private:
        virtual bool OnUserCreate() override {

        }

        virtual bool OnUserUpdate(float fElapsedTime) override {

        }
    };

    void gameLoop() {

    }
}