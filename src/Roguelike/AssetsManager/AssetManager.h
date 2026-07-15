//
// Created by YangKai.Lin on 2026/7/15.
//

#ifndef TOYPROJECTS_ROGUELIKE_ASSETMANAGER_H
#define TOYPROJECTS_ROGUELIKE_ASSETMANAGER_H

#include <map>
#include <string>
#include "../TextureManager/textureManager.h"
#include "../App/Vector2D.h"
#include "../ECS/ECS.h"
#include "SDL_ttf.h"

namespace roguelike {
    class AssetManager {
    public:
        AssetManager(ECS::Manager* man);
        ~AssetManager();

        void CreateProjectile(calculate::Vector2D pos, calculate::Vector2D vel, int range, int speed, std::string id);
        void AddTexture(std::string id, const char* path);
        SDL_Texture* GetTexture(std::string id);
        void AddFont(std::string id, std::string path, int fontSize);
        TTF_Font* GetFont(std::string id);

    private:
        ECS::Manager* manager;
        std::map<std::string, SDL_Texture*> textures;
        std::map<std::string, TTF_Font*> fonts;
    };
}


#endif //TOYPROJECTS_ROGUELIKE_ASSETMANAGER_H