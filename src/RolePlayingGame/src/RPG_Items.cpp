#include "RPG_Items.h"
#include "RPG_Engine.h"

namespace RoleGame {
    RPG_Engine *cItem::g_engine = nullptr;

    cItem::cItem(std::string name, olcSprite *sprite, std::string desc) {
        sName = name;
        pSprite = sprite;
        sDescription = desc;
    }


    cItem_Health::cItem_Health() : cItem("Small Health", RPG_Assets::get().GetSprite("health"), "Restores 10 health") {
    }


    bool cItem_Health::OnInteract(cDynamic *object) {
        OnUse(object);
        return false; // Just absorb
    }


    bool cItem_Health::OnUse(cDynamic *object) {
        if (object != nullptr) {
            cDynamic_Creature *dyn = (cDynamic_Creature *) object;
            dyn->nHealth = min(dyn->nHealth + 10, dyn->nHealthMax);
        }
        return true;
    }


    cItem_HealthBoost::cItem_HealthBoost() : cItem("Health Boost", RPG_Assets::get().GetSprite("healthboost"),
                                                   "Increases Max Health by 10") {
    }

    bool cItem_HealthBoost::OnInteract(cDynamic *object) {
        return true; // Add to inventory
    }

    bool cItem_HealthBoost::OnUse(cDynamic *object) {
        if (object != nullptr) {
            cDynamic_Creature *dyn = (cDynamic_Creature *) object;
            dyn->nHealthMax += 10;
            dyn->nHealth = dyn->nHealthMax;
        }

        return true; // Remove from inventory
    }

    cWeapon::cWeapon(std::string name, olcSprite *sprite, std::string desc, int dmg) : cItem(name, sprite, desc) {
        nDamage = dmg;
    }

    bool cWeapon::OnInteract(cDynamic *object) {
        return false;
    }

    bool cWeapon::OnUse(cDynamic *object) {
        return false;
    }


    cWeapon_Sword::cWeapon_Sword() : cWeapon("Basic Sword", RPG_Assets::get().GetSprite("Basic Sword"),
                                             "A wooden sword, 5 dmg", 5) {
    }

    bool cWeapon_Sword::OnUse(cDynamic *object) {
        // When weapons are used, they are used on the object that owns the weapon, i.e.
        // the attacker. However this does not imply the attacker attacks themselves

        // Get direction of attacker
        cDynamic_Creature *aggressor = (cDynamic_Creature *) object;

        // Determine attack origin
        float x, y, vx, vy;
        if (aggressor->GetFacingDirection() == 0) // South
        {
            x = aggressor->px;
            y = aggressor->py + 1.0f;
            vx = 0.0f;
            vy = 1.0f;
        }

        if (aggressor->GetFacingDirection() == 1) // East
        {
            x = aggressor->px - 1.0f;
            y = aggressor->py;
            vx = -1.0f;
            vy = 0.0f;
        }

        if (aggressor->GetFacingDirection() == 2) // North
        {
            x = aggressor->px;
            y = aggressor->py - 1.0f;
            vx = 0.0f;
            vy = -1.0f;
        }

        if (aggressor->GetFacingDirection() == 3) // West
        {
            x = aggressor->px + 1.0f;
            y = aggressor->py;
            vx = 1.0f;
            vy = 0.0f;
        }

        if (aggressor->nHealth == aggressor->nHealthMax) {
            // Beam sword
            cDynamic_Projectile *p = new cDynamic_Projectile(x, y, aggressor->bFriendly, vx * 15.0f, vy * 15.0f, 1.0f,
                                                             RPG_Assets::get().GetSprite("Basic Sword"),
                                                             (aggressor->GetFacingDirection() + 3) % 4 + 1, 1.0f);
            p->bSolidVsMap = true;
            p->bSolidVsDyn = false;
            p->nDamage = 5;
            p->bOneHit = false;
            g_engine->AddProjectile(p);
        }

        cDynamic_Projectile *p = new cDynamic_Projectile(x, y, aggressor->bFriendly, aggressor->vx, aggressor->vy, 0.1f,
                                                         RPG_Assets::get().GetSprite("Basic Sword"),
                                                         (aggressor->GetFacingDirection() + 3) % 4 + 1, 0.0f);
        p->bSolidVsMap = false;
        p->bSolidVsDyn = false;
        p->nDamage = 5;
        p->bOneHit = true;

        g_engine->AddProjectile(p);

        return false;
    }
}