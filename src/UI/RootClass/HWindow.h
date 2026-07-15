//
// Created by 15813 on 2026/7/14.
//

#ifndef TOYPROJECTS_UI_HWINDOW_H
#define TOYPROJECTS_UI_HWINDOW_H

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#ifdef UIPlugin
#define UIPlugin_API __declspec(dllexport)
#else
#define UIPlugin_API __declspec(dllimport)
#endif

namespace UI {
    namespace Window {
        extern "C" UIPlugin_API GLFWwindow* HWindowsInit(const char* WindowTitle, int width, int height);
        extern "C" UIPlugin_API void HWindowsFrameInit();
        extern "C" UIPlugin_API void HWindowsFrameEnd();
    }
}

#endif //TOYPROJECTS_UI_HWINDOW_H
