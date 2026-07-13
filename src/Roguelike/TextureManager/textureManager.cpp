//
// Created by YangKai.Lin on 2026/7/12.
//

#include "textureManager.h"

namespace roguelike {
    SDL_Texture * TextureManager::LoadTexture(const char *filename) {
        SDL_Surface *tempSurface = IMG_Load(filename);
        SDL_Texture* tex = SDL_CreateTextureFromSurface(Game::renderer.get_window_renderer(), tempSurface);
        SDL_FreeSurface(tempSurface);
        return tex;
    }
}
