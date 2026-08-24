//
// Created by 15813 on 2026/8/5.
//

#ifndef TOYPROJECTS_UI_IMGUI_001_GUI_H
#define TOYPROJECTS_UI_IMGUI_001_GUI_H

#include "PictureWall.h"

namespace GUI{

    void begin(){
        PictureWall::begin();
    }

    void update(){
        if (ImGui::Begin("Test Window")) {
            PictureWall::update();
        }
        ImGui::End();
    }
}

#endif //TOYPROJECTS_UI_IMGUI_001_GUI_H
