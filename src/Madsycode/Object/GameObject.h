//
// Created by YangKai.Lin on 2026/7/18.
//

#ifndef TOYPROJECTS_Madsycode_GAMEOBJECT_H
#define TOYPROJECTS_Madsycode_GAMEOBJECT_H
#include <SDL_render.h>

#include "IObject.h"
#include "../Physics/Transform.h"

namespace Madsycode::Object {

    struct Properties {
    public:
        Properties(std::string textureID, int x, int y, int width, int height, SDL_RendererFlip flip = SDL_FLIP_NONE) : TextureID(textureID), X(x), Y(y), Width(width), Height(height), Flip(flip) {}
        std::string TextureID;
        int Width, Height;
        float X, Y;
        SDL_RendererFlip Flip;
    };

    class GameObject : public IObject{
    public:
        GameObject(Properties* properties) : m_TextureID(properties->TextureID), m_Width(properties->Width), m_Height(properties->Height), m_Flip(properties->Flip) {
            m_Transform = new Physics::Transform(properties->X, properties->Y);
        };

        void Draw() override{}

        void Update(float dt) override{}

        void Clean() override{}
    protected:
        using Type = int;
        Physics::Transform* m_Transform;
        int m_Width, m_Height;
        std::string m_TextureID;
        SDL_RendererFlip m_Flip;
    };
}

#endif //TOYPROJECTS_Madsycode_GAMEOBJECT_H