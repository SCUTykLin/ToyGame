//
// Created by YangKai.Lin on 2026/7/13.
//

#include "map.h"
#include "../TextureManager/textureManager.h"

namespace roguelike {
    Map::Map() {
        dirt = TextureManager::LoadTexture("src/Roguelike/Assets/terrain_ss.png");
        grass = TextureManager::LoadTexture("src/Roguelike/Assets/terrain_ss.png");
        water = TextureManager::LoadTexture("src/Roguelike/Assets/terrain_ss.png");
    }

    Map::~Map() {
    }

    void Map::LoadMap() {
    }

    void Map::DrawMap() {
    }
}