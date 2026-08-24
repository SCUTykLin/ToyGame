//
// Created by 15813 on 2026/8/4.
//

#ifndef TOYPROJECTS_UI_IMGUI_001_PICTUREWALL_ITEM_H
#define TOYPROJECTS_UI_IMGUI_001_PICTUREWALL_ITEM_H

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

namespace PictureWall{

    struct PictureInfo {
        ImTextureID texture = 0;
        int h = 0, w = 0;
        bool Direction = false;
        float Proportion = -1;
        PictureInfo(){}
        PictureInfo(ImTextureID tex, int h, int w);
    };

    bool PictureItem(const PictureInfo& info, const char* label, float& size_x);
}


#endif //TOYPROJECTS_UI_IMGUI_001_PICTUREWALL_ITEM_H
