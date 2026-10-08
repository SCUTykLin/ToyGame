#pragma once

#include "RPG_Dynamic.h"

namespace RoleGame {
    class RPG_Engine;
    class cItem {
    public:
        cItem(std::string name, olcSprite *sprite, std::string desc);

        virtual bool OnInteract(cDynamic *object) { return false; }
        virtual bool OnUse(cDynamic *object) { return false; }

    public:
        std::string sName;
        std::string sDescription;
        olcSprite *pSprite;
        bool bKeyItem = false;
        bool bEquipable = false;

        static RPG_Engine *g_engine;
    };


    class cItem_Health : public cItem // give player 10hp
    {
    public:
        cItem_Health();

        bool OnInteract(cDynamic *object) override;

        bool OnUse(cDynamic *object) override;
    };

    class cItem_HealthBoost : public cItem // raise max hp 10
    {
    public:
        cItem_HealthBoost();

        bool OnInteract(cDynamic *object) override;

        bool OnUse(cDynamic *object) override;
    };


    class cWeapon : public cItem {
    public:
        cWeapon(std::string name, olcSprite *sprite, std::string desc, int dmg);

        bool OnInteract(cDynamic *object) override;

        bool OnUse(cDynamic *object) override;

    public:
        int nDamage = 0;
    };


    class cWeapon_Sword : public cWeapon {
    public:
        cWeapon_Sword();

    public:
        bool OnUse(cDynamic *object) override;
    };
}