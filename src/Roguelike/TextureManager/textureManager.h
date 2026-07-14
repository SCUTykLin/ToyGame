//
// Created by YangKai.Lin on 2026/7/12.
//

#ifndef TOYPROJECTS_ROGUELIKE_TEXTUREMANAGER_H
#define TOYPROJECTS_ROGUELIKE_TEXTUREMANAGER_H

#include "SDL2/SDL.h"
#include "SDL_image.h"

namespace roguelike {
    class TextureManager {
    public:
        static SDL_Texture* LoadTexture(const char* filename);
    };
}


#endif //TOYPROJECTS_ROGUELIKE_TEXTUREMANAGER_H