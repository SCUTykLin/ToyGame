//
// Created by 15813 on 2026/8/5.
//

#ifndef TOYPROJECTS_GUI_H
#define TOYPROJECTS_GUI_H

#include "PictureWall_Item.h"

namespace GUI{
    void begin(){

    }

    void update(){
        static float size_buffer = 600;
        if (ImGui::Begin("test window")) {
            PictureWall::PictureItem(0, "test Item", size_buffer);
        }
        ImGui::End();
    }
}

#endif //TOYPROJECTS_GUI_H
