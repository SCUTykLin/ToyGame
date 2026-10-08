#pragma once
#include "RPG_Assets.h"
#include "olcConsoleGameEngine.h"

namespace RoleGame {
	class cItem;
	class RPG_Engine;
	class cWeapon;

	class cDynamic
	{
	public:
		cDynamic(std::string n);
		~cDynamic();

	public:
		float px, py;
		float vx, vy;
		bool bSolidVsMap;
		bool bSolidVsDyn;
		bool bFriendly;
		bool bRedundant;
		bool bIsProjectile;
		bool bIsAttackable;
		std::string sName;

	public:
		virtual void DrawSelf(olcConsoleGameEngine *gfx, float ox, float oy) {}
		virtual void Update(float fElapsedTime, cDynamic* player = nullptr) {}
		virtual void OnInteract(cDynamic* player = nullptr) {}

		static RPG_Engine* g_engine;
	};



	class cDynamic_Creature : public cDynamic
	{
	public:
		cDynamic_Creature(std::string n, olcSprite *sprite);

	protected:
		olcSprite *m_pSprite;
		float m_fTimer;
		int m_nGraphicCounter;
		enum { SOUTH = 0, WEST = 1, NORTH = 2, EAST = 3 } m_nFacingDirection;
		enum { STANDING, WALKING, CELEBRATING, DEAD } m_nGraphicState;

	public:
		int nHealth;
		int nHealthMax;
		int bControllable = true;

	public:
		void DrawSelf(olcConsoleGameEngine *gfx, float ox, float oy) override;
		void Update(float fElapsedTime, cDynamic* player = nullptr) override;
		virtual void Behaviour(float fElapsedTime, cDynamic* player = nullptr);
		int GetFacingDirection() { return m_nFacingDirection; };
		virtual void PerformAttack() {};
		void KnockBack(float dx, float dy, float dist);

		cWeapon *pEquipedWeapon = nullptr;

	protected:
		float m_fStateTick;
		float m_fKnockBackTimer = 0.0f;
		float m_fKnockBackDX = 0.0f;
		float m_fKnockBackDY = 0.0f;

	};


	class cDynamic_Creature_Skelly : public cDynamic_Creature
	{
	public:
		cDynamic_Creature_Skelly();

		void Behaviour(float fElapsedTime, cDynamic* player = nullptr) override;
		void PerformAttack() override;
	};

	class cDynamic_Creature_Witty : public cDynamic_Creature
	{
	public:
		cDynamic_Creature_Witty();

	public:
		void PerformAttack() override;
	};





	class cDynamic_Teleport : public cDynamic
	{
	public:
		cDynamic_Teleport(float x, float y, std::string sMapName, float tx, float ty);
		void DrawSelf(olcConsoleGameEngine *gfx, float ox, float oy) override;
		void Update(float fElapsedTime, cDynamic* player = nullptr) override;

	public:
		std::string sMapName;
		float fMapPosX;
		float fMapPosY;
	};

	class cDynamic_Item : public cDynamic
	{
	public:
		cDynamic_Item(float x, float y, cItem* item);
		void DrawSelf(olcConsoleGameEngine *gfx, float ox, float oy) override;
		void OnInteract(cDynamic* player = nullptr) override;

	public:
		cItem * item;
		bool bCollected = false;
	};

	class cDynamic_Projectile : public cDynamic
	{
	public:
		cDynamic_Projectile(float ox, float oy, bool bFriend, float velx, float vely, float duration, olcSprite *sprite, float tx, float ty);
		void DrawSelf(olcConsoleGameEngine *gfx, float ox, float oy) override;
		void Update(float fElapsedTime, cDynamic* player = nullptr) override;

	public:
		olcSprite * pSprite = nullptr;
		float fSpriteX;
		float fSpriteY;
		float fDuration;
		bool bOneHit = true;
		int nDamage = 0;
	};
}