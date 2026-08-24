//
// Created by YangKai.Lin on 2026/7/19.
//

#ifndef TOYPROJECTS_Madsycode_INPUTS_H
#define TOYPROJECTS_Madsycode_INPUTS_H

#include <SDL2/SDL.h>

namespace Madsycode::Event {
    enum Axis {HORIZONTAL, VERTICAL};

    class Input {
    public:
        static Input* GetInstance() {
            return s_Instance = (s_Instance != nullptr) ? s_Instance : new Input();
        }

        void Listen();
        bool GetKeyDown(SDL_Scancode key);

        int GetAxisKey(Axis axis);
    private:
        Input();
        void KeyUp();
        void KeyDown();
        const Uint8* m_KeyStates;
        static Input* s_Instance;
    };
}


#endif //TOYPROJECTS_Madsycode_INPUTS_H