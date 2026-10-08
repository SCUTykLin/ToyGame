#pragma once
#include "olcConsoleGameEngine.h"
#include <map>

namespace RoleGame {
	class cMap;
	class cItem;

	class RPG_Assets
	{
	public:
		static RPG_Assets& get()
		{
			static RPG_Assets me;
			return me;
		}

		RPG_Assets(RPG_Assets const&) = delete;
		void operator=(RPG_Assets const&) = delete;

		olcSprite* GetSprite(std::string name)
		{
			return m_mapSprites[name];
		}

		cMap* GetMap(std::string name)
		{
			return m_mapMaps[name];
		}

		cItem* GetItem(std::string name)
		{
			return m_mapItems[name];
		}

		void LoadSprites();
		void LoadMaps();
		void LoadItems();


	private:
		RPG_Assets();
		~RPG_Assets();

		std::map<std::string, olcSprite*> m_mapSprites;
		std::map<std::string, cMap*> m_mapMaps;
		std::map<std::string, cItem*> m_mapItems;
	};
}