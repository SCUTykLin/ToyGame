//
// Created by 15813 on 2026/7/16.
//

#include "ECS.h"

namespace roguelike::ECS {
    void Entity::addGroup(Group mGroup) {
        groupBitSet[mGroup] = true;
        manager.AddToGroup(this, mGroup);
    }
}