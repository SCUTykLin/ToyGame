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

#define RUN_FORCE 4.0f
#define ATTACK_TIME 30.0f

namespace Madsycode::Character {
    class Warrior : public Character {
    public:
        Warrior(Object::Properties* props);

        void Draw() override;

        void Update(float dt) override;

        void Clean() override;
    private:

        void AnimationState();

        bool m_IsRunning;
        bool m_IsJumping;
        bool m_IsFalling;
        bool m_IsGrounded;
        bool m_IsAttacking;
        bool m_IsCrouching;

        float m_JumpTime;
        float m_JumpForce;
        float m_AttackTime;

        bool m_JumpKeyWasDown = false; // 是否是刚刚按下
        bool m_AttackKeyWasDown = false;

        Physics::Collider* m_Collider;
        Animation::Animation* m_Animation;
        Physics::RigidBody* m_RigidBody;
        Physics::Vector2D<float> m_LastSafePosition;
    };
}

#endif //TOYPROJECTS_Madsycode_WARRIOR_H