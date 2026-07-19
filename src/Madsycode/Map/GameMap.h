//
// Created by YangKai.Lin on 2026/7/19.
//

#ifndef TOYPROJECTS_Madsycode_GAMEMAP_H
#define TOYPROJECTS_Madsycode_GAMEMAP_H

#include <vector>
#include "Layer.h"

namespace Madsycode::Map {
    class GameMap {
    public:
        void Render() {
            for (unsigned int i = 0; i < m_MapLayers.size(); ++i) {
                m_MapLayers[i]->Render();
            }
        }

        void Update() {
            for (unsigned int i = 0; i < m_MapLayers.size(); ++i) {
                m_MapLayers[i]->Update();
            }
        }

        auto GetMapLayers() {return m_MapLayers;}
    private:
        friend class MapParser;
        std::vector<Layer*> m_MapLayers;
    };
}

#endif //TOYPROJECTS_Madsycode_GAMEMAP_H