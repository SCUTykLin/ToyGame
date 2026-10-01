//
// Created by YangKai.Lin on 2026/9/30.
//

#include "RPG_Maps.h"
#include <fstream>

#include "RPG_Assets.h"


namespace RoleGame {
    cMap::cMap() : pSprite(nullptr), nWidth(0), nHeight(0), m_solids(nullptr), m_indices(nullptr) {
    }

    cMap::~cMap() {
        delete[] m_solids;
        delete[] m_indices;
    }

    bool cMap::GetSolid(int x, int y) {
        if (x >= 0 && x < nWidth && y >= 0 && y < nHeight) return m_solids[y * nWidth + x];
        else return true;
    }

    int cMap::GetIndex(int x, int y) {
        if (x >= 0 && x < nWidth && y >= 0 && y < nHeight) {
            return m_indices[y * nWidth + x];
        } else {
            return 0;
        }
    }

    bool cMap::Create(const std::string &fileData, olcSprite *sprite, std::string name) {
        sName = std::move(name);
        pSprite = sprite;
        std::ifstream data(fileData, std::ios::in | std::ios::binary);
        if (data.is_open()) {
            data >> nWidth >> nHeight;
            m_solids = new bool[nWidth * nHeight];
            m_indices = new int[nWidth * nHeight];
            for (int i = 0; i < nWidth * nHeight; i++) {
                data >> m_indices[i];
                data >> m_solids[i];
            }
            return true;
        }
        return false;
    }

    cMap_Village1::cMap_Village1() {
        Create("src/RolePlayingGame/assets/map/village1.lvl", RPG_Assets::get().GetSprite("village"), "coder town");
    }
}
