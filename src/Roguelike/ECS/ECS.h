//
// Created by 15813 on 2026/7/13.
//

#ifndef TOYPROJECTS_ROGUELIKE_ECS_H
#define TOYPROJECTS_ROGUELIKE_ECS_H

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <bitset>
#include <array>

namespace roguelike::ECS {
    class Component;

    class Entity;

    class Manager;

    using ComponentID = std::size_t;
    using Group = std::size_t;

    inline ComponentID getNewComponentTypeID() { // generate component id.
        static ComponentID lastID = 0u;
        return lastID++;
    }

/*
 * 每一个类型的实体会包含唯一一个id
 * */
    template<typename T>
    inline ComponentID getComponentTypeID() noexcept {
        static ComponentID typeID = getComponentTypeID();
        return typeID;
    }

    constexpr std::size_t maxComponents = 32;
    constexpr std::size_t maxGroups = 32;
/*
 * 这里ComponentArray和components职责不同。components是负责查找，而ComponentArray是为了管理释放
 * */

    using ComponentBitSet = std::bitset<maxComponents>; // 存储组件id
    using GroupBitSet = std::bitset<maxGroups>;
    using ComponentArray = std::array<Component *, maxComponents>; // 存储组件

    class Component { // Component基类
    public:
        Entity *entity;

        virtual void init() {};

        virtual void update() {};

        virtual void draw() {};

        virtual ~Component() {};
    };

    class Entity {
    private:
        Manager& manager;
        bool active = true;
        std::vector<std::unique_ptr<Component>> components;
        ComponentArray componentArray;
        ComponentBitSet componentBitSet;
        GroupBitSet groupBitSet;
    public:
        Entity(Manager& manager) : manager(manager) {}
        void update() {
            for (auto &c: components)
                c->update();
        }

        void draw() {
            for (auto &c: components)
                c->draw();
        }

        bool isActive() const { return active; }

        void destroy() {
            active = false;
        }

        bool hasGroup(Group mGroup) {
            return groupBitSet[mGroup];
        }

        void addGroup(Group mGroup) {

        }

        void delGroup(Group mGroup) {
            groupBitSet[mGroup] = false;
        }

        template<typename T>
        bool hasComponent() const {
            return componentBitSet[getComponentTypeID<T>()];
        }

        template<typename T, typename ... TArgs>
        T &addComponent(TArgs &&... mArgs) {
            T *c(new T(std::forward<TArgs>(mArgs)...));
            c->entity = this;
            std::unique_ptr<Component> uPtr{c};
            components.emplace_back(std::move(uPtr));
            componentArray[getComponentTypeID<T>()] = c;
            componentBitSet[getComponentTypeID<T>()] = true;

            c->init();
            return *c;
        }

        template<typename T>
        T &getComponent() const {
            auto ptr(componentArray[getComponentTypeID<T>()]);
            return *static_cast<T *>(ptr);
        };
    };

    class Manager {
    private:
        std::vector<std::unique_ptr<Entity>> entities;
        std::array<std::vector<Entity*>, maxGroups> groupedEntities;
    public:
        void update() {
            for (auto &e: entities) {
                e->update();
            }
        }

        void draw() {
            for (auto &e: entities) {
                e->draw();
            }
        }

        void refresh() {

            for (auto i(0u); i < maxGroups; ++i) {
                auto& v(groupedEntities[i]);
                v.erase(std::remove_if(std::begin(v), std::end(v),
                    [i](Entity *e) {
                        return !e->isActive() || !e->hasGroup(i);
                    }), std::end(v));
            }

            entities.erase(std::remove_if(std::begin(entities),
                                          std::end(entities),
                                          [](const std::unique_ptr<Entity> &mEntity) {
                                              return !mEntity->isActive();
                                          }), std::end(entities));
        }

        Entity &addEntity() {
            Entity *e = new Entity(*this);
            std::unique_ptr<Entity> uPtr{e};
            entities.emplace_back(std::move(uPtr));
            return *e;
        }
    };
}
#endif //TOYPROJECTS_ROGUELIKE_ECS_H
