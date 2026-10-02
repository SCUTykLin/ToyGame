//
// Created by YangKai.Lin on 2026/10/1.
//

#ifndef TOYPROJECTS_RoleGame_RPG_DYNAMIC_H
#define TOYPROJECTS_RoleGame_RPG_DYNAMIC_H

#include "olcConsoleGameEngine.h"
#include <string>

namespace RoleGame {
    class cDynamic {
    public:
        cDynamic(std::string n);
        ~cDynamic();
    public:
        float px, py;
        float vx, vy;
        bool bSolidVsMap;
        bool bSolidVsDyn;
        bool bFriendly;
        std::string sName;

    public:
        virtual void DrawSelf(olcConsoleGameEngine* gfx, float ox, float oy) {}
        virtual void Update(float fElapsedTime) {}
    };

    class cDynamic_Creature : public cDynamic {
    public:
        cDynamic_Creature(std::string n, olcSprite* sprite);

    protected:
        olcSprite* m_pSprite;
        float m_fTimer;
        int m_nGraphicCounter = 0;
        enum {
            SOUTH = 0,
            WEST = 1,
            NORTH = 2,
            EAST = 3,
        } m_nFacingDirection;

        enum {
            STANDING,
            WALKING,
            CELEBRATING,
            DEAD
        } m_nGraphicState;

    public:
        int nHealth;
        int nHealthMax;

    public:
        void DrawSelf(olcConsoleGameEngine *gfx, float ox, float oy) override;
        void Update(float fElapsedTime) override;
    };

}

#endif //TOYPROJECTS_RoleGame_RPG_DYNAMIC_H
