//
// Created by YangKai.Lin on 2026/7/16.
//

#include "app.h"
#include "SDL2/SDL.h"
#include <iostream>
#include "../Core/Engine.h"

namespace Madsycode {
    void App::run() {
        Engine::Engine::GetInstance()->Init();
        while (Engine::Engine::GetInstance()->isRunning()){
            Engine::Engine::GetInstance()->Events();
            Engine::Engine::GetInstance()->Update();
            Engine::Engine::GetInstance()->Render();
        }
        Engine::Engine::GetInstance()->Clean();
    }
}
