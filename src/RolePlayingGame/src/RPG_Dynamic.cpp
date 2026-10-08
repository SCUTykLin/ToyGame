
#include "RPG_Dynamic.h"
#include "RPG_Engine.h"

namespace RoleGame {
	RPG_Engine* cDynamic::g_engine = nullptr;

	cDynamic::cDynamic(std::string n)
	{
		sName = n;
		px = 0.0f;
		py = 0.0f;
		vx = 0.0f;
		vy = 0.0f;
		bSolidVsMap = true;
		bSolidVsDyn = true;
		bFriendly = true;
		bRedundant = false;
		bIsAttackable = false;
		bIsProjectile = false;
	}

	cDynamic::~cDynamic()
	{

	}


	/////////////////////////////////////////////////////////////////////

	cDynamic_Creature::cDynamic_Creature(std::string name, olcSprite *sprite) : cDynamic(name)
	{
		m_pSprite = sprite;
		nHealth = 10;
		nHealthMax = 10;
		m_nFacingDirection = SOUTH;
		m_nGraphicState = STANDING;
		m_nGraphicCounter = 0;
		m_fTimer = 0.0f;
		bIsAttackable = true;
	}

	void cDynamic_Creature::Update(float fElapsedTime, cDynamic* player)
	{
		if (m_fKnockBackTimer > 0.0f)
		{
			vx = m_fKnockBackDX * 10.0f;
			vy = m_fKnockBackDY * 10.0f;
			bIsAttackable = false;
			m_fKnockBackTimer -= fElapsedTime;
			if (m_fKnockBackTimer <= 0.0f)
			{
				m_fStateTick = 0.0f;
				bControllable = true;
				bIsAttackable = true;
			}

		}
		else
		{
			bSolidVsDyn = true;
			m_fTimer += fElapsedTime;
			if (m_fTimer >= 0.2f)
			{
				m_fTimer -= 0.2f;
				m_nGraphicCounter++;
				m_nGraphicCounter %= 2;
			}

			if (fabs(vx) > 0 || fabs(vy) > 0)
				m_nGraphicState = WALKING;
			else
				m_nGraphicState = STANDING;

			if (nHealth <= 0)
				m_nGraphicState = DEAD;

			if (vx < 0.0f) m_nFacingDirection = WEST;
			if (vx > 0.0f) m_nFacingDirection = EAST;
			if (vy < -0.0f) m_nFacingDirection = NORTH;
			if (vy > 0.0f) m_nFacingDirection = SOUTH;

			Behaviour(fElapsedTime, player);
		}
	}

	void cDynamic_Creature::KnockBack(float dx, float dy, float dist)
	{
		m_fKnockBackDX = dx;
		m_fKnockBackDY = dy;
		m_fKnockBackTimer = dist;
		bSolidVsDyn = false;
		bControllable = false;
		bIsAttackable = false;
	}


	void cDynamic_Creature::DrawSelf(olcConsoleGameEngine *gfx, float ox, float oy)
	{
		int nSheetOffsetX = 0;
		int nSheetOffsetY = 0;

		switch (m_nGraphicState)
		{
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

		gfx->DrawPartialSprite((px - ox) * 16.0f, (py - oy)*16.0f, m_pSprite, nSheetOffsetX, nSheetOffsetY, 16, 16);
	}

	void cDynamic_Creature::Behaviour(float fElapsedTime, cDynamic* player)
	{
		// No default behaviour
	}




	cDynamic_Creature_Witty::cDynamic_Creature_Witty() : cDynamic_Creature("witty", RPG_Assets::get().GetSprite("player"))
	{
		bFriendly = true;
		nHealth = 9;
		nHealthMax = 10;
		m_fStateTick = 2.0f;
		pEquipedWeapon = (cWeapon*)RPG_Assets::get().GetItem("Basic Sword");
	}

	void cDynamic_Creature_Witty::PerformAttack()
	{
		if (pEquipedWeapon == nullptr)
			return;

		pEquipedWeapon->OnUse(this);
	}




	cDynamic_Creature_Skelly::cDynamic_Creature_Skelly() : cDynamic_Creature("Skelly", RPG_Assets::get().GetSprite("skelly"))
	{
		bFriendly = false;
		nHealth = 10;
		nHealthMax = 10;
		m_fStateTick = 2.0f;

		pEquipedWeapon = (cWeapon*)RPG_Assets::get().GetItem("Basic Sword");
	}


	void cDynamic_Creature_Skelly::Behaviour(float fElapsedTime, cDynamic* player)
	{
		if (nHealth <= 0)
		{
			vx = 0;
			vy = 0;
			bSolidVsDyn = false;
			bIsAttackable = false;
			return;
		}

		// Check if player is nearby
		float fTargetX = player->px - px;
		float fTargetY = player->py - py;
		float fDistance = sqrtf(fTargetX*fTargetX + fTargetY * fTargetY);

		m_fStateTick -= fElapsedTime;

		if (m_fStateTick <= 0.0f)
		{
			if (fDistance < 6.0f)
			{
				vx = (fTargetX / fDistance) * 2.0f;
				vy = (fTargetY / fDistance) * 2.0f;

				if (fDistance < 1.5f)
					PerformAttack();
			}
			else
			{
				vx = 0;
				vy = 0;
			}

			m_fStateTick += 1.0f;
		}
	}

	void cDynamic_Creature_Skelly::PerformAttack()
	{
		if (pEquipedWeapon == nullptr)
			return;

		pEquipedWeapon->OnUse(this);
	}







	cDynamic_Teleport::cDynamic_Teleport(float x, float y, std::string mapName, float tx, float ty) : cDynamic("Teleport")
	{
		px = x;
		py = y;
		fMapPosX = tx;
		fMapPosY = ty;
		sMapName = mapName;
		bSolidVsDyn = false;
		bSolidVsMap = false;
	}

	void cDynamic_Teleport::DrawSelf(olcConsoleGameEngine *gfx, float ox, float oy)
	{
		// Does Nothing
		gfx->DrawCircle(((px + 0.5f) - ox) * 16.0f, ((py + 0.5f) - oy) * 16.0f, 0.5f * 16.0f); // For debugging
	}

	void cDynamic_Teleport::Update(float fElapsedTime, cDynamic* player)
	{
		// Does Nothing
	}


	cDynamic_Item::cDynamic_Item(float x, float y, cItem* i) : cDynamic("pickup")
	{
		px = x;
		py = y;
		bSolidVsDyn = false;
		bSolidVsMap = false;
		bFriendly = true;
		bCollected = false;
		item = i;
	}

	void cDynamic_Item::DrawSelf(olcConsoleGameEngine *gfx, float ox, float oy)
	{
		if (bCollected)
			return;

		gfx->DrawPartialSprite((px - ox) * 16.0f, (py - oy)*16.0f, item->pSprite, 0, 0, 16, 16);
	}

	void cDynamic_Item::OnInteract(cDynamic* player)
	{
		if (bCollected)
			return;

		if (item->OnInteract(player))
		{
			// Add item to inventory
			g_engine->GiveItem(item);
		}

		bCollected = true;
	}

	cDynamic_Projectile::cDynamic_Projectile(float ox, float oy, bool bFriend, float velx, float vely, float duration, olcSprite *sprite, float tx, float ty) : cDynamic("projectile")
	{
		pSprite = sprite;
		fSpriteX = tx;
		fSpriteY = ty;
		fDuration = duration;
		px = ox;
		py = oy;
		vx = velx;
		vy = vely;
		bSolidVsDyn = false;
		bSolidVsMap = true;
		bIsProjectile = true;
		bIsAttackable = false;
		bFriendly = bFriend;
	}

	void cDynamic_Projectile::DrawSelf(olcConsoleGameEngine *gfx, float ox, float oy)
	{
		gfx->DrawPartialSprite((px - ox) * 16, (py - oy) * 16, pSprite, fSpriteX * 16, fSpriteY * 16, 16, 16);

	}

	void cDynamic_Projectile::Update(float fElapsedTime, cDynamic* player)
	{
		fDuration -= fElapsedTime;
		if (fDuration <= 0.0f)
			bRedundant = true;
	}
}