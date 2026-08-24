//
// Created by YangKai.Lin on 2026/7/13.
//

#ifndef TOYPROJECTS_ROGUELIKE_MAP_H
#define TOYPROJECTS_ROGUELIKE_MAP_H
#include "../Game/game.h"
namespace roguelike {
    class Map {
    public:
        Map(std::string& tID, int ms, int ts);
        Map(std::string&& tID, int ms, int ts);
        ~Map();

        void LoadMap(const std::string&& path, int sizeX, int sizeY);
        void AddTile(int srcX, int srcY, int xpos, int ypos);
    private:
        std::string texID;
        int mapScale;
        int tileSize;
        int scaledSize;
    };
}


#endif //TOYPROJECTS_ROGUELIKE_MAP_H