//
// Created by 15813 on 2026/7/19.
//

#ifndef TOYPROJECTS_Madsycode_CAMERA_H
#define TOYPROJECTS_Madsycode_CAMERA_H

#include "SDL2/SDL.h"
#include "../Physics/Point.h"
#include "../Physics/Vector2D.h"
#include "../Assets/Animation_Config.h"

namespace Madsycode::Camera {
    class Camera {
    public:
        void Update(float dt);
        inline static Camera* GetInstance(){return s_Instance = (s_Instance != nullptr) ? s_Instance : new Camera();}
        inline SDL_Rect GetViewBox() {return m_ViewBox;}
        inline Physics::Vector2D<float> GetPosition() {return m_Position;}
        inline void SetTarget(Physics::Point* target){m_Target = target;}
    private:
        Camera(){m_ViewBox = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};}
        Physics::Point* m_Target; // center of player
        Physics::Vector2D<float> m_Position;
        SDL_Rect m_ViewBox;

        static Camera* s_Instance;
    };
}


#endif //TOYPROJECTS_Madsycode_CAMERA_H
