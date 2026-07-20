//
// Created by YangKai.Lin on 2026/7/19.
//

#ifndef TOYPROJECTS_Madsycode_TIME_H
#define TOYPROJECTS_Madsycode_TIME_H

constexpr int TARGET_FPS = 60;
constexpr float TARGET_DELTATIME = 1.5f;

namespace Madsycode::Time {
    class Timer {
    public:
        void Tick();
        inline float GetDeltaTime() const { return m_DeltaTime; }
        inline static Timer* GetInstance() { return s_Instance = (s_Instance != nullptr) ? s_Instance : new Timer(); }
    private:
        Timer() : m_DeltaTime(0.0f), m_LastTime(0.0f) {};
        static Timer* s_Instance;
        float m_DeltaTime;
        float m_LastTime;
    };
}


#endif //TOYPROJECTS_Madsycode_TIME_H