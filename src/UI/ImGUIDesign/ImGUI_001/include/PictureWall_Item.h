//
// Created by 15813 on 2026/8/4.
//

#ifndef TOYPROJECTS_PICTUREWALL_ITEM_H
#define TOYPROJECTS_PICTUREWALL_ITEM_H

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

namespace PictureWall{
    bool PictureItem(ImTextureID texture, const char* label, float& size_x);
}


#endif //TOYPROJECTS_PICTUREWALL_ITEM_H
