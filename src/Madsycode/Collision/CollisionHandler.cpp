//
// Created by 15813 on 2026/7/19.
//

#include "CollisionHandler.h"
#include "../Core/Engine.h"

namespace Madsycode::Collision{
    CollisionHandler* CollisionHandler::s_Instance = nullptr;
    bool CollisionHandler::CheckCollision(SDL_Rect a, SDL_Rect b){
        bool x_overlaps = (a.x < b.x + b.w) && (a.x + a.w > b.w);
        bool y_overlaps = (a.y < b.y + b.h) && (a.y + a.h > b.y);
        return x_overlaps && y_overlaps;
    }

    bool CollisionHandler::MapCollision (SDL_Rect a){
        int tilesize = 32;
        int Rowcount = 20;
        int Colcount = 60;

        int left_tile = a.x / tilesize;
        int right_tile = (a.x + a.w) / tilesize;
        int top_tile = a.y / tilesize;
        int bottom_tile = (a.y + a.h) / tilesize;

        if (left_tile < 0)
            left_tile = 0;
        if (right_tile > Colcount)
            right_tile = Colcount;
        if (top_tile < 0)
            top_tile = 0;
        if (top_tile > Rowcount)
            top_tile = Rowcount;

        for (int i = left_tile; i <= right_tile; ++i)
            for (int j = top_tile; j <= bottom_tile; ++j)
                if (m_CollisionTilemap[j][i] > 0)
                    return true;

        return false;
    }

    CollisionHandler::CollisionHandler(){
        m_CollisionLayer = (Map::TileLayer*)(Engine::Engine::GetInstance()->GetMap()->GetMapLayers().back());
        m_CollisionTilemap = m_CollisionLayer->GetTileMap();
    }
}