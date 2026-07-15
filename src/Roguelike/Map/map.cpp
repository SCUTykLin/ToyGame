//
// Created by YangKai.Lin on 2026/7/13.
//

#include "map.h"
#include "../TextureManager/textureManager.h"
#include <fstream>
#include "../ECS/ECS.h"
#include "../Game/game.h"
#include "../ECS/Components.h"

namespace roguelike {

    extern Manager manager;

    Map::Map(std::string& tID, int ms, int ts) : texID(tID), mapScale(ms), tileSize(ts) {
        scaledSize = ms * ts;
    }
    Map::~Map() {
    }

    void Map::LoadMap(std::string& path, int sizeX, int sizeY)
    {
        char c;
        std::fstream mapFile;
        mapFile.open(path);

        int srcX, srcY;

        for (int y = 0; y < sizeY; y++)
        {
            for (int x = 0; x < sizeX; x++)
            {
                mapFile.get(c);
                srcY = atoi(&c) * tileSize;
                mapFile.get(c);
                srcX = atoi(&c) * tileSize;
                AddTile(srcX, srcY, x * scaledSize, y * scaledSize);
                mapFile.ignore();
            }
        }

        mapFile.ignore();

        for (int y = 0; y < sizeY; y++)
        {
            for (int x = 0; x < sizeX; x++)
            {
                mapFile.get(c);
                if (c == '1')
                {
                    auto& tcol(manager.addEntity());
                    tcol.addComponent<ECS::ColliderComponent>("terrain", x * scaledSize, y * scaledSize, scaledSize);
                    tcol.addGroup(Game::groupColliders);
                }
                mapFile.ignore();
            }
        }

        mapFile.close();
    }

}