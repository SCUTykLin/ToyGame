//
// Created by YangKai.Lin on 2026/7/16.
//

#ifndef TOYPROJECTS_RPG_Platformer_APP_H
#define TOYPROJECTS_RPG_Platformer_APP_H

#include <string>

namespace RPG_Platformer {
    const std::string ASSET_DIR = "src/RPG_Platformer/Assets/gfx/";
    class App {
    public:
        void run();
        void initWindow();
    };
}


#endif //TOYPROJECTS_RPG_Platformer_APP_H