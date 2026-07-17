//
// Created by YangKai.Lin on 2026/7/17.
//

#ifndef TOYPROJECTS_Madsycode_TEXTUREMANAGER_H
#define TOYPROJECTS_Madsycode_TEXTUREMANAGER_H

#include <SDL_render.h>
#include <string>

namespace Madsycode::Texture {
    class TextureManager {
    public:
        TextureManager();
        static TextureManager* GetInstance(){return s_Instance = (s_Instance != nullptr) ? s_Instance : new TextureManager();}
        bool Load(const std::string& id, const std::string& filename);
        void Drop(const std::string& id);
        void Clean();
        void Draw(const std::string& id, int x, int y, int width, int height, SDL_RendererFlip flip=SDL_FLIP_NONE);
    private:
        TextureManager(){};
        static TextureManager* s_Instance;
    };
}


#endif //TOYPROJECTS_Madsycode_TEXTUREMANAGER_H