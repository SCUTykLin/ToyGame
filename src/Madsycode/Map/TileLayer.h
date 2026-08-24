//
// Created by YangKai.Lin on 2026/7/19.
//

#ifndef TOYPROJECTS_Madsycode_TILELAYER_H
#define TOYPROJECTS_Madsycode_TILELAYER_H

#include "Layer.h"
#include <string>
#include <vector>

namespace Madsycode::Map {

    struct Tileset {
        int FirstID, LastID;
        int RowCount, ColCount;
        int TileCount, TileSize;
        std::string Name, Source;
    };

    using TilesetList = std::vector<Tileset>;
    using TileMap = std::vector<std::vector<int>>;

    class TileLayer : public Layer {
    public:
        TileLayer(int tilesize, int rowcount, int colcount, TileMap tilemap, TilesetList tilesets);
        virtual void Render();
        virtual void Update();
        inline TileMap GetTileMap() const { return m_TileMap; }
    private:
        int m_TileSize;
        int m_RowCount, m_ColCount;
        TileMap m_TileMap;
        TilesetList m_Tilesets;
    };
}


#endif //TOYPROJECTS_Madsycode_TILELAYER_H