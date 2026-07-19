//
// Created by YangKai.Lin on 2026/7/17.
//

#ifndef TOYPROJECTS_Madsycode_TEXTUREMANAGER_H
#define TOYPROJECTS_Madsycode_TEXTUREMANAGER_H

#include <SDL_render.h>
#include <string>
#include <map>

namespace Madsycode::Texture {
    class TextureManager {
    public:
        static TextureManager* GetInstance(){return s_Instance = (s_Instance != nullptr) ? s_Instance : new TextureManager();}
        bool Load(const std::string& id, const std::string& filename);
        void Drop(const std::string& id);
        void Clean();
        void Draw(const std::string& id, int x, int y, int width, int height, SDL_RendererFlip flip=SDL_FLIP_NONE);
        void DrawTile(const std::string& tilesetID, int tileSize, int x, int y, int row, int frame, SDL_RendererFlip=SDL_FLIP_NONE);
        void DrawFrame(const std::string& id, int x, int y, int width, int height, int row, int frame, SDL_RendererFlip flip=SDL_FLIP_NONE);
    private:
        TextureManager(){};
        std::map<std::string, SDL_Texture*> m_Textures;
        static TextureManager* s_Instance;
    };
}


#endif //TOYPROJECTS_Madsycode_TEXTUREMANAGER_H