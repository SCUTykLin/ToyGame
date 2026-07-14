//
// Created by 15813 on 2026/7/13.
//

#ifndef TOYPROJECTS_ROGUELIKE_SPRITECOMPONENT_H
#define TOYPROJECTS_ROGUELIKE_SPRITECOMPONENT_H

#include "../TextureManager/textureManager.h"
#include "TransformComponent.h"
#include "SDL2/SDL.h"

namespace roguelike::ECS {
    class SpriteComponent : public Component {
    private:
        TransformComponent *transform;
        SDL_Texture *texture;
        SDL_Rect srcRect, destRect;

    public:
        SpriteComponent() = default;
        ~SpriteComponent(){
            SDL_DestroyTexture(texture);
        }
        SpriteComponent(const char *path) {
            setTex(path);
        }

        SDL_Texture *getTexture() {
            return texture;
        }

        SDL_Rect &get_srcRect() {
            return srcRect;
        }

        SDL_Rect &get_destRect() {
            return destRect;
        }

        void setTex(const char *path) {
            texture = roguelike::TextureManager::LoadTexture(path);
        }

        void init() override {

            transform = &entity->getComponent<TransformComponent>();
            srcRect.x = srcRect.y = 0;
            srcRect.w = transform->width;
            srcRect.h = transform->height;
        }

        void update() override {
            destRect.x = static_cast<int>(transform->getPos().x);
            destRect.y = static_cast<int>(transform->getPos().y);
            destRect.w = transform->width * transform->scale;
            destRect.h = transform->height * transform->scale;
        }

        void draw() override {

        }

    };
}
#endif //TOYPROJECTS_ROGUELIKE_SPRITECOMPONENT_H
