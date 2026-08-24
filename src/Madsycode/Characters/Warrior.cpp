//
// Created by YangKai.Lin on 2026/7/18.
//

#include "Warrior.h"

#include <iostream>
#include "../Assets/Animation_Config.h"
#include "../Graphics/TextureManager.h"
#include "../Inputs/Inputs.h"
#include <SDL2/SDL.h>

#include "../Camera/Camera.h"
#include "../Collision/CollisionHandler.h"
#include "../Core/Engine.h"

namespace Madsycode::Character {
    Warrior::Warrior(Object::Properties* props) : Character(props) {

        m_IsWalking = false;
        m_IsRunning = false;
        m_IsJumping = false;
        m_IsFalling = false;
        m_IsGrounded = false;
        m_IsAttacking = false;
        m_IsCrouching = false;

        m_Flip = SDL_FLIP_NONE;
        m_JumpTime = JUMP_TIME;
        m_JumpForce = JUMP_FORCE;
        m_AttackTime = ATTACK_TIME;

        m_Collider = new Physics::Collider();
        m_Collider->SetBuffer(-150, -110, 300, 220);

        m_RigidBody = new Physics::RigidBody();
        m_RigidBody->SetGravity(GRAVITY);

        m_Animation = new Animation::Animation();
        m_Animation->SetProps(m_TextureID, 1, 8, 100);

    }
    void Warrior::Draw() {
        m_Animation->Draw(m_Transform->X, m_Transform->Y, m_Width, m_Height, m_Flip);
#if __DEFBUG__ == 1
        Physics::Vector2D<float> cam = Camera::Camera::GetInstance()->GetPosition();
        SDL_Rect box = m_Collider->Get();
        box.x -= cam.X;
        box.y -= cam.Y;
        SDL_RenderDrawRect(Engine::Engine::GetInstance()->GetRenderer(), &box);
#endif
    }

    void Warrior::Update(float dt)
    {
        auto* input = Event::Input::GetInstance();

        const int horizontal = input->GetAxisKey(Event::HORIZONTAL);

        const bool crouchHeld = input->GetKeyDown(SDL_SCANCODE_S);
        const bool jumpHeld   = input->GetKeyDown(SDL_SCANCODE_J);
        const bool attackHeld = input->GetKeyDown(SDL_SCANCODE_K);

        // 如果输入系统没有 GetKeyPressed，就用这种方式判断“刚刚按下”
        const bool jumpPressed  = jumpHeld && !m_JumpKeyWasDown;
        const bool attackPressed = attackHeld && !m_AttackKeyWasDown;

        m_JumpKeyWasDown = jumpHeld;
        m_AttackKeyWasDown = attackHeld;

        // 每帧重置临时状态和外力
        m_IsRunning = false;
        m_IsWalking = false;
        m_IsCrouching = false;
        m_RigidBody->UnSetForce();

        // --------------------
        // 攻击状态
        // --------------------
        if (attackPressed && !m_IsAttacking)
        {
            m_IsAttacking = true;
            m_AttackTime = ATTACK_TIME;

            // 如果攻击时要求立即停下，需要清除水平速度。
            // 根据你的 RigidBody 接口替换这个函数。
            m_RigidBody->UnSetForce();
        }

        if (m_IsAttacking)
        {
            m_AttackTime -= dt;

            if (m_AttackTime <= 0.0f)
            {
                m_IsAttacking = false;
                m_AttackTime = ATTACK_TIME;
            }
        }

        // --------------------
        // 蹲下
        // --------------------
        m_IsCrouching = crouchHeld && m_IsGrounded && !m_IsAttacking;

        if (m_IsCrouching){
            m_RigidBody->UnSetForce();
        }

        // --------------------
        // 水平移动
        // --------------------
        if (!m_IsAttacking && !m_IsCrouching){
            if (horizontal == FORWARD){
                m_Flip = SDL_FLIP_NONE;
                if (Event::Input::GetInstance()->GetKeyDown(SDL_SCANCODE_LCTRL)) {
                    m_IsRunning = m_IsGrounded;
                    m_RigidBody->ApplyForceX(FORWARD * RUN_FORCE);
                }else {
                    m_IsWalking = m_IsGrounded;
                    m_RigidBody->ApplyForceX(FORWARD * WALK_FORCE);
                }
            }
            else if (horizontal == BACKWARD){
                m_RigidBody->ApplyForceX(BACKWARD * RUN_FORCE);
                m_Flip = SDL_FLIP_HORIZONTAL;
                if (Event::Input::GetInstance()->GetKeyDown(SDL_SCANCODE_LCTRL)) {
                    m_IsRunning = m_IsGrounded;
                    m_RigidBody->ApplyForceX(BACKWARD * RUN_FORCE);
                }else {
                    m_IsWalking = m_IsGrounded;
                    m_RigidBody->ApplyForceX(BACKWARD * WALK_FORCE);
                }
            }
        }

        // --------------------
        // 跳跃
        // --------------------
        if (jumpPressed && m_IsGrounded && !m_IsCrouching && !m_IsAttacking){
            m_IsJumping = true;
            m_IsGrounded = false;
            m_JumpTime = JUMP_TIME;

            m_RigidBody->ApplyForceY(UPWARD * m_JumpForce);
        }
        else if (jumpHeld && m_IsJumping && m_JumpTime > 0.0f)
        {
            // 按住 J 时继续施加向上的力，实现长按跳得更高
            m_JumpTime -= dt;
            m_RigidBody->ApplyForceY(UPWARD * m_JumpForce);
        }
        else if (!jumpHeld || m_JumpTime <= 0.0f)
        {
            // 松开跳跃键只结束持续跳跃，不要在这里设置 Grounded
            m_IsJumping = false;
        }

        // 每帧只更新一次刚体
        m_RigidBody->Update(dt);

        const float moveX = m_RigidBody->Position().X;
        const float moveY = m_RigidBody->Position().Y;

        // --------------------
        // X 轴移动与碰撞
        // --------------------
        m_LastSafePosition.X = m_Transform->X;
        m_Transform->X += moveX;

        m_Collider->Set(m_Transform->X, m_Transform->Y, AssetConfig::pixel_width,AssetConfig::pixel_height);

        if (Collision::CollisionHandler::GetInstance()
                ->MapCollision(m_Collider->Get()))
        {
            m_Transform->X = m_LastSafePosition.X;

            m_Collider->Set(m_Transform->X, m_Transform->Y,AssetConfig::pixel_width,AssetConfig::pixel_height);

            m_RigidBody->UnSetForce();
        }

        // --------------------
        // Y 轴移动与碰撞
        // --------------------
        m_LastSafePosition.Y = m_Transform->Y;
        m_Transform->Y += moveY;

        m_Collider->Set(m_Transform->X, m_Transform->Y,AssetConfig::pixel_width,AssetConfig::pixel_height);

        const bool verticalCollision = Collision::CollisionHandler::GetInstance()->MapCollision(m_Collider->Get());

        if (verticalCollision){
            m_Transform->Y = m_LastSafePosition.Y;

            m_Collider->Set(m_Transform->X, m_Transform->Y,AssetConfig::pixel_width,AssetConfig::pixel_height);

            // Y 正方向向下，因此 moveY > 0 表示落地碰撞
            if (moveY > 0.0f)
            {
                m_IsGrounded = true;
                m_IsJumping = false;
                m_JumpTime = JUMP_TIME;
            }
            else
            {
                // 向上碰到天花板，不应该设置为着地
                m_IsGrounded = false;
                m_IsJumping = false;
            }

            m_RigidBody->UnSetForce();
        }
        else
        {
            // 向下移动 1 像素进行地面探测。
            // 解决角色静止时没有 Y 位移，无法判断是否站在地面的问题。
            m_Collider->Set(
                m_Transform->X,
                m_Transform->Y + 1.0f,
                AssetConfig::pixel_width,
                AssetConfig::pixel_height
            );

            m_IsGrounded =
                Collision::CollisionHandler::GetInstance()
                    ->MapCollision(m_Collider->Get());

            // 恢复碰撞箱位置
            m_Collider->Set(
                m_Transform->X,
                m_Transform->Y,
                AssetConfig::pixel_width,
                AssetConfig::pixel_height
            );
        }

        // 下落状态应该在移动和碰撞处理之后计算
        m_IsFalling =
            !m_IsGrounded &&
            m_RigidBody->Velocity().Y > 0.0f;

        m_Origin->X = m_Transform->X + m_Width * 0.5f;
        m_Origin->Y = m_Transform->Y + m_Height * 0.5f;

        AnimationState();
        m_Animation->Update();
    }

    // void Warrior::Update(float dt) {
    //     m_IsRunning = false;
    //     m_IsCrouching = false;
    //     m_RigidBody->UnSetForce();
    //
    //     // Running forward
    //     if (Event::Input::GetInstance()->GetAxisKey(Event::HORIZONTAL) == FORWARD && !m_IsAttacking) {
    //         m_RigidBody->ApplyForceX(FORWARD * RUN_FORCE);
    //         m_Flip = SDL_FLIP_NONE;
    //         m_IsRunning = true;
    //     }
    //     // Running backward
    //     if (Event::Input::GetInstance()->GetAxisKey(Event::HORIZONTAL) == BACKWARD && !m_IsAttacking) {
    //         m_RigidBody->ApplyForceX(BACKWARD * RUN_FORCE);
    //         m_Flip = SDL_FLIP_HORIZONTAL;
    //         m_IsRunning = true;
    //     }
    //
    //     // Crouch
    //     if (Event::Input::GetInstance()->GetKeyDown(SDL_SCANCODE_S)) {
    //         m_RigidBody->UnSetForce();
    //         m_IsCrouching = true;
    //     }
    //     // Attacking
    //     if (Event::Input::GetInstance()->GetKeyDown(SDL_SCANCODE_K)) {
    //         m_RigidBody->UnSetForce();
    //         m_IsAttacking = true;
    //     }
    //     // Jump
    //     if (Event::Input::GetInstance()->GetKeyDown(SDL_SCANCODE_J) && m_IsGrounded) {
    //         m_IsJumping = true;
    //         m_IsGrounded = false;
    //         m_RigidBody->ApplyForceY(UPWARD * m_JumpForce);
    //     }
    //     if (Event::Input::GetInstance()->GetKeyDown(SDL_SCANCODE_J) && m_IsJumping && m_JumpTime > 0) {
    //         m_JumpTime -= dt;
    //         m_RigidBody->ApplyForceY(UPWARD * m_JumpForce);
    //     }else {
    //         m_IsJumping = false;
    //         m_IsGrounded = true;
    //         m_JumpTime = JUMP_TIME;
    //     }
    //
    //     // Falling
    //     if (m_RigidBody->Velocity().Y > 0 && !m_IsGrounded) {
    //         m_IsFalling = true;
    //     }else {
    //         m_IsFalling = false;
    //     }
    //
    //     // Attack timer
    //     if (m_IsAttacking && m_AttackTime > 0) {
    //         m_AttackTime -= dt;
    //     }else {
    //         m_IsAttacking = false;
    //         m_AttackTime = ATTACK_TIME;
    //     }
    //
    //     // move x axis
    //     m_RigidBody->Update(dt);
    //     m_LastSafePosition.X = m_Transform->X;
    //     m_Transform->X += m_RigidBody->Position().X;
    //     m_Collider->Set(m_Transform->X, m_Transform->Y, AssetConfig::pixel_width, AssetConfig::pixel_height);
    //
    //     if (Collision::CollisionHandler::GetInstance()->MapCollision(m_Collider->Get())) {
    //         m_Transform->X = m_LastSafePosition.X;
    //     }
    //
    //     // move y axis
    //     m_RigidBody->Update(dt);
    //     m_LastSafePosition.Y = m_Transform->Y;
    //     m_Transform->Y += m_RigidBody->Position().Y;
    //     m_Collider->Set(m_Transform->X, m_Transform->Y, AssetConfig::pixel_width, AssetConfig::pixel_height);
    //
    //     if (Collision::CollisionHandler::GetInstance()->MapCollision(m_Collider->Get())) {
    //         m_IsGrounded = true;
    //         m_Transform->Y = m_LastSafePosition.Y;
    //     }else {
    //         m_IsGrounded = false;
    //     }
    //
    //     m_Origin->X = m_Transform->X + m_Width / 2;
    //     m_Origin->Y = m_Transform->Y + m_Height / 2;
    //
    //     AnimationState();
    //     m_Animation->Update();
    // }

    void Warrior::Clean() {
        Texture::TextureManager::GetInstance()->Clean();
    }

    void Warrior::AnimationState() {
        m_Animation->SetProps("idle", 1, 8, 100);
        if (m_IsJumping) {
            m_Animation->SetProps("jump", 1, 8, 100);
        }else if (m_IsCrouching) {
            m_Animation->SetProps("crouch", 1, 5, 200);
        }else if(m_IsWalking){
            m_Animation->SetProps("walk", 1, 8, 100);
        }else if (m_IsRunning) {
            m_Animation->SetProps("run", 1, 8, 100);
        }else if (m_IsFalling) {
            m_Animation->SetProps("fall", 1, 9, 100);
        }else if (m_IsAttacking) {
            m_Animation->SetProps("attack", 1, 7, 100);
        }
    }
}
