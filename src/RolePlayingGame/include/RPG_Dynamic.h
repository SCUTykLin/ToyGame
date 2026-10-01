//
// Created by YangKai.Lin on 2026/10/1.
//

#ifndef TOYPROJECTS_RoleGame_RPG_DYNAMIC_H
#define TOYPROJECTS_RoleGame_RPG_DYNAMIC_H

#include <string>

namespace RoleGame {
    class cDynamic {
    public:
        cDynamic();
        ~cDynamic();
    public:
        float px, py;
        float vx, vy;
        bool bSolidVsMap;
        bool bSolidVsDyn;
        bool bFriendly;
        std::string sName;
    };
}


#endif //TOYPROJECTS_RoleGame_RPG_DYNAMIC_H
