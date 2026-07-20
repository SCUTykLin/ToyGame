//
// Created by YangKai.Lin on 2026/7/18.
//

#ifndef TOYPROJECTS_Madsycode_WARRIOR_H
#define TOYPROJECTS_Madsycode_WARRIOR_H

#include "Characters.h"
#include "../Animation/Animation.h"
#include "../Physics/RigidBody.h"
#include "../Physics/Collider.h"

#define JUMP_TIME 15.0f
#define JUMP_FORCE 10.0f

namespace Madsycode::Character {
    class Warrior : public Character {
    public:
        Warrior(Object::Properties* props);

        void Draw() override;

        void Update(float dt) override;

        void Clean() override;
    private:
        bool m_IsJumping;
        bool m_IsGrounded;
        float m_JumpTime;
        float m_JumpForce;

        Physics::Collider* m_Collider;
        Animation::Animation* m_Animation;
        Physics::RigidBody* m_RigidBody;
        Physics::Vector2D<float> m_LastSafePosition;
    };
}

#endif //TOYPROJECTS_Madsycode_WARRIOR_H