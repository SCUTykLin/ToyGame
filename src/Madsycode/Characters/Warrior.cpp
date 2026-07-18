//
// Created by YangKai.Lin on 2026/7/18.
//

#include "Warrior.h"
#include "../Graphics/TextureManager.h"
#include <SDL2/SDL.h>

namespace Madsycode::Character {
    Warrior::Warrior(Object::Properties* props) : Character(props) {
        m_Animation = new Animation::Animation();
        m_RigidBody = new Physics::RigidBody();
        m_Animation->SetProps(m_TextureID, 1, 12, 80, SDL_FLIP_HORIZONTAL);
    }

    void Warrior::Draw() {
        m_Animation->Draw(m_Transform->X, m_Transform->Y, m_Width, m_Height);
    }

    void Warrior::Update(float dt) {
        m_RigidBody->Update(1);
        m_Transform->TranslateX(m_RigidBody->Position().X);
        m_Transform->TranslateY(m_RigidBody->Position().Y);
        m_Animation->Update();
    }

    void Warrior::Clean() {
        Texture::TextureManager::GetInstance()->Clean();
    }
}