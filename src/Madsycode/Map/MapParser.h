//
// Created by YangKai.Lin on 2026/7/19.
//

#ifndef TOYPROJECTS_Madsycode_MAPPARSER_H
#define TOYPROJECTS_Madsycode_MAPPARSER_H

#include <map>
#include <string>
#include "GameMap.h"
#include "TileLayer.h"
#include "tinyxml.h"

namespace Madsycode::Map{
    class MapParser {
    public:
        bool Load();
        void clean();

        inline GameMap* GetMaps(const std::string& id) {return m_MapDict[id];}
        inline static MapParser* GetInstance() {return s_Instance = (s_Instance != nullptr) ? s_Instance : new MapParser;}
    private:
        bool Parse(const std::string& id, const std::string& source);
        Tileset ParseTileset(TiXmlElement* xmlTileset);
        TileLayer* ParseTileLayer(TiXmlElement* xmlLayer, std::vector<Tileset> tilesets, int tilesize, int rowcount, int colcount);

        MapParser(){};
        static MapParser* s_Instance;
        std::map<std::string, GameMap*> m_MapDict;
    };
}


#endif //TOYPROJECTS_Madsycode_MAPPARSER_H