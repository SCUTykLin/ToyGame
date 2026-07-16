//
// Created by 15813 on 2026/7/17.
//

#ifndef TOYPROJECTS_ENGINE_H
#define TOYPROJECTS_ENGINE_H

namespace Madsycode::Engine {
    class Engine {
    public:
        static Engine* GetInstance(){
            return s_Instance = (s_Instance != nullptr) ? s_Instance : new Engine();
        }
        bool Init();
        void Clean();
        void Quit();
        void Update();
        void Render();
        void Events();

        inline bool isRunning(){return m_IsRunning;}
    private:
        Engine(){};
        bool m_IsRunning;
        static Engine* s_Instance;
    };
}


#endif //TOYPROJECTS_ENGINE_H
