//
// Created by YangKai.Lin on 2026/7/15.
//

#ifndef TOYPROJECTS_ROGUELIKE_UILABEL_H
#define TOYPROJECTS_ROGUELIKE_UILABEL_H

#include "ECS.h"
#include "../AssetsManager/AssetManager.h"
#include "../Game/game.h"
#include "SDL2/SDL.h"
#include <SDL_ttf.h>
#include <string>

namespace roguelike {
    class UILabel : public ECS::Component {
    public:
        UILabel(int xpos, int ypos, std::string text, std::string font, SDL_Color& colour) : labelText(text), labelFont(font), textColour(colour)
        {
            position.x = xpos;
            position.y = ypos;

            SetLabelText(labelText, labelFont);
        }

        void SetLabelText(std::string text, std::string font)
        {
            SDL_Surface* surf = TTF_RenderText_Blended(Game::assets->GetFont(font), text.c_str(), textColour);
            labelTexture = SDL_CreateTextureFromSurface(Game::renderer.get_window_renderer(), surf);
            SDL_FreeSurface(surf);

            SDL_QueryTexture(labelTexture, nullptr, nullptr, &position.w, &position.h);
        }


        void draw() override
        {
            SDL_RenderCopy(Game::renderer, labelTexture, nullptr, &position);
        }

        ~UILabel()
        {}
    private:
        SDL_Rect position;
        std::string labelText;
        std::string labelFont;
        SDL_Color textColour;
        SDL_Texture* labelTexture;
    };
}
#endif //TOYPROJECTS_ROGUELIKE_UILABEL_H