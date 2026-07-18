//
// Created by YangKai.Lin on 2026/7/18.
//

#ifndef TOYPROJECTS_Madsycode_WARRIOR_H
#define TOYPROJECTS_Madsycode_WARRIOR_H

#include "Characters.h"
#include "../Animation/Animation.h"
#include "../Physics/RigidBody.h"

namespace Madsycode::Character {
    class Warrior : public Character {
    public:
        Warrior(Object::Properties* props);

        void Draw() override;

        void Update(float dt) override;

        void Clean() override;
    private:
        Animation::Animation* m_Animation;
        Physics::RigidBody* m_RigidBody;
    };
}

#endif //TOYPROJECTS_Madsycode_WARRIOR_H