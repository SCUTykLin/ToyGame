//
// Created by YangKai.Lin on 2026/7/19.
//

#include "Inputs.h"
#include "../Core/Engine.h"

namespace Madsycode::Event {
    void Input::Listen() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: Engine::Engine::GetInstance()->Quit(); break;
            }
        }
    }

    bool Input::GetKeyDown(SDL_Scancode key) {
    }

    Input::Input() {
    }

    void Input::KeyUp() {
    }

    void Input::KeyDown() {
    }
}
