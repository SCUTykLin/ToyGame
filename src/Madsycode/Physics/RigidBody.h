//
// Created by YangKai.Lin on 2026/7/19.
//

#ifndef TOYPROJECTS_Madsycode_RIGIDBODY_H
#define TOYPROJECTS_Madsycode_RIGIDBODY_H

#include "Vector2D.h"

#define UNI_MASS 1.0f
#define GRAVITY 3.0f // 9.81f

#define FORWARD 1
#define BACKWARD -1
#define UPWARD -1
#define DOWNWARD 1

namespace Madsycode::Physics {

    class RigidBody {
    public:
        RigidBody() : m_Mass(UNI_MASS), m_Gravity(GRAVITY) {
            m_Force.Zero();
            m_Friction.Zero();
            m_Position.Zero();
            m_Velocity.Zero();
            m_Acceleration.Zero();
        }

        // Setter Gravity & Mass
        inline void SetMass(float mass) { m_Mass = mass; }
        inline void SetGravity(float g) { m_Gravity = g; }

        // Force
        inline void ApplyForce(Vector2D<float> F) {m_Force = F;}
        inline void ApplyForceX(float Fx) {m_Force.X = Fx;}
        inline void ApplyForceY(float Fy) {m_Force.Y = Fy;}
        inline void UnSetForce(){m_Force = Vector2D{0.0f, 0.0f};}

        // Friction
        inline void ApplyFriction(Vector2D<float> Fr){m_Force = Fr;}
        inline void UnSetFriction(){m_Friction = Vector2D{0.0f, 0.0f};}

        // Getters
        inline float GetMass() const { return m_Mass; }
        inline auto Position() const { return m_Position; }
        inline auto Velocity() const { return m_Velocity; }
        inline auto Acceleration() const { return m_Acceleration; }

        // Update methods
        void Update(float dt) {
            m_Acceleration.X = (m_Force.X + m_Friction.X) / m_Mass;
            m_Acceleration.Y = m_Gravity + m_Force.Y / m_Mass;
            m_Velocity = m_Acceleration * dt;
            m_Position = m_Velocity * dt;
        }

    private:
        float m_Mass;
        float m_Gravity;

        Vector2D<float> m_Force;
        Vector2D<float> m_Friction;
        Vector2D<float> m_Position;
        Vector2D<float> m_Velocity;
        Vector2D<float> m_Acceleration;
    };
}

#endif //TOYPROJECTS_RIGIDBODY_H