//
// Created by YangKai.Lin on 2026/8/5.
//

#ifndef TOYPROJECTS_UI_IMGUI_001_IMAGELOADER_H
#define TOYPROJECTS_UI_IMGUI_001_IMAGELOADER_H
#include <imgui.h>
namespace HImage {
    ImTextureID HTextureLoader(const char* filename, int* out_width, int* out_height);
    void HTextureUnload(ImTextureID tex);
}

#endif //TOYPROJECTS_UI_IMGUI_001_IMAGELOADER_H