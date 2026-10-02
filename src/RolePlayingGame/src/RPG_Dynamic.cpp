//
// Created by YangKai.Lin on 2026/10/1.
//

#include "RPG_Dynamic.h"
#include <string>

namespace RoleGame {
    cDynamic::cDynamic(std::string n) : sName(n), px(0.0f), py(0.0f), vx(0.0f), vy(0.0f), bSolidVsMap(true), bSolidVsDyn(true), bFriendly(true) {}

    cDynamic::~cDynamic() {}

    cDynamic_Creature::cDynamic_Creature(std::string n, olcSprite *sprite) : cDynamic(n), m_pSprite(sprite), nHealth(10), nHealthMax(10), m_nFacingDirection(SOUTH), m_nGraphicState(STANDING), m_fTimer(0.0f), m_nGraphicCounter(0) {}

    void cDynamic_Creature::Update(float fElapsedTime) {
        m_fTimer += fElapsedTime;
        if (m_fTimer >= 0.2f) {
            m_fTimer -= 0.2f;
            m_nGraphicCounter ++;
            m_nGraphicCounter %= 2;
        }

        if (fabs(vx) > 0 || fabs(vy) > 0) {
            m_nGraphicState = WALKING;
        }else {
            m_nGraphicState = STANDING;
        }

        if (nHealth <= 0) {
            m_nGraphicState = DEAD;
        }

        if (vx < -0.1f) m_nFacingDirection = WEST;
        if (vx > 0.1f) m_nFacingDirection = EAST;
        if (vy < -0.1f) m_nFacingDirection = NORTH;
        if (vy > 0.1f) m_nFacingDirection = SOUTH;

    }

    void cDynamic_Creature::DrawSelf(olcConsoleGameEngine *gfx, float ox, float oy) {
        int nSheetOffsetX = 0;
        int nSheetOffsetY = 0;

        switch (m_nGraphicState) {
            case STANDING:
                nSheetOffsetX = m_nFacingDirection * 16;
                break;

            case WALKING:
                nSheetOffsetX = m_nFacingDirection * 16;
                nSheetOffsetY = m_nGraphicCounter * 16;
                break;

            case CELEBRATING:
                nSheetOffsetX = 4 * 16;
                break;

            case DEAD:
                nSheetOffsetX = 4 * 16;
                nSheetOffsetY = 1 * 16;
                break;
        }

        gfx->DrawPartialSprite((px - ox) * 16.0f, (py - oy) * 16.0f, m_pSprite, nSheetOffsetX, nSheetOffsetY, 16, 16);

    }
}
