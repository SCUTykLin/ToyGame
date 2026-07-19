//
// Created by YangKai.Lin on 2026/7/19.
//

#include "Inputs.h"
#include "../Core/Engine.h"

namespace Madsycode::Event {
    Input* Input::s_Instance = nullptr;
    void Input::Listen() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: Engine::Engine::GetInstance()->Quit(); break;
                case SDL_KEYDOWN: KeyDown(); break;
                case SDL_KEYUP: KeyUp(); break;
            }
        }
    }

    bool Input::GetKeyDown(SDL_Scancode key) {
        return (m_KeyStates[key] == 1);
    }

    Input::Input() {
        m_KeyStates = SDL_GetKeyboardState(nullptr);
    }

    void Input::KeyUp() {
        m_KeyStates = SDL_GetKeyboardState(nullptr);
    }

    void Input::KeyDown() {
        m_KeyStates = SDL_GetKeyboardState(nullptr);
    }
}
