//
// Created by 15813 on 2026/7/14.
//

#ifndef TOYPROJECTS_ROGUELIKE_TILECOMPONENT_H
#define TOYPROJECTS_ROGUELIKE_TILECOMPONENT_H

#include "ECS.h"
#include "../App/Vector2D.h"
#include "../Game/game.h"
#include "../TextureManager/textureManager.h"

namespace roguelike {
    class TileComponent : public ECS::Component{
    public:
        SDL_Texture * texture;
        SDL_Rect srcRect, destRect;
        calculate::Vector2D position;

        TileComponent() = default;

        ~TileComponent()
        {
            SDL_DestroyTexture(texture);
        }


        TileComponent(int srcX, int srcY, int xpos, int ypos, int tsize, int tscale, std::string id)
        {
            texture = Game::assets->GetTexture(id);

            srcRect.x = srcX;
            srcRect.y = srcY;
            srcRect.w = srcRect.h = tsize;
            position.x = static_cast<float>(xpos);
            position.y = static_cast<float>(ypos);
            destRect.w = destRect.h = tsize * tscale;
        }

        void draw() override
        {
            TextureManager::Draw(texture, srcRect, destRect, SDL_FLIP_NONE);
        }

    };
}
#endif //TOYPROJECTS_ROGUELIKE_TILECOMPONENT_H
