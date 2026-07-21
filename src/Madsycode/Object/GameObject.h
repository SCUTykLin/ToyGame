//
// Created by YangKai.Lin on 2026/7/18.
//

#ifndef TOYPROJECTS_Madsycode_GAMEOBJECT_H
#define TOYPROJECTS_Madsycode_GAMEOBJECT_H
#include <SDL_render.h>

#include "IObject.h"
#include "../Physics/Transform.h"
#include "../Physics/Point.h"

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

            float px = properties->X + m_Width / 2;
            float py = properties->Y + m_Height / 2;
            m_Origin = new Physics::Point(px, py);
        };

        inline Physics::Point* GetOrigin(){return m_Origin;}

        void Draw() override{}

        void Update(float dt) override{}

        void Clean() override{}
    protected:
        Physics::Point* m_Origin;
        Physics::Transform* m_Transform;
        int m_Width, m_Height;
        std::string m_TextureID;
        SDL_RendererFlip m_Flip;
    };
}

#endif //TOYPROJECTS_Madsycode_GAMEOBJECT_H