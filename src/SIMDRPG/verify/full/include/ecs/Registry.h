#pragma once
#include <bitset>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ecs {

using Entity = uint32_t;

// 无效实体。注意：0 是**合法**实体 id（第一个 create() 就返回 0），
// 所以不能用 0 表示"失败"，否则第一个生成的敌人会被误判。
inline constexpr Entity INVALID_ENTITY = 0xFFFFFFFFu;

// 组件的唯一要求：能默认构造（池子要预分配）
template <typename T>
concept Component = std::is_default_constructible_v<T>;

class Registry {
public:
    static constexpr size_t MAX_COMPONENTS = 32;
    using Mask = std::bitset<MAX_COMPONENTS>;

    Entity create() {
        const Entity e = next_++;
        if (masks_.size() <= e) masks_.resize(size_t(e) + 1);
        return e;
    }

    template <Component T, typename... Args>
    T& add(Entity e, Args&&... args) {
        auto& pool = pool_of<T>();
        if (pool.size() <= e) pool.resize(size_t(e) + 1);
        pool[e] = T{ std::forward<Args>(args)... };
        masks_[e].set(cid<T>());
        return pool[e];
    }

    template <Component T>
    T* get(Entity e) {
        if (e >= masks_.size() || !masks_[e].test(cid<T>())) return nullptr;
        auto& pool = pool_of<T>();
        if (e >= pool.size()) return nullptr;
        return &pool[e];
    }

    template <Component T>
    bool has(Entity e) const {
        return e < masks_.size() && masks_[e].test(cid<T>());
    }

    // 销毁 = 清掉所有组件位。组件内存留着（池化，下次复用）。
    void destroy(Entity e) {
        if (e < masks_.size()) masks_[e].reset();
    }

    void clear() {
        pools_.clear();
        masks_.clear();
        next_ = 0;
    }

    Entity count() const { return next_; }

    // 遍历拥有全部 Ts... 的实体
    template <Component... Ts, typename F>
    void each(F&& f) {
        for (Entity e = 0; e < next_; ++e) {
            if (e >= masks_.size()) break;
            if (masks_[e].none()) continue;          // 已销毁，跳过
            if ((masks_[e].test(cid<Ts>()) && ...)) {
                f(e, *get<Ts>(e)...);
            }
        }
    }

private:
    struct PoolBase { virtual ~PoolBase() = default; };
    template <typename T> struct Pool : PoolBase { std::vector<T> data; };

    template <typename T>
    static size_t cid() {
        static const size_t id = next_id_++;
        return id;
    }

    template <typename T>
    std::vector<T>& pool_of() {
        const size_t id = cid<T>();
        auto it = pools_.find(id);
        if (it == pools_.end()) {
            it = pools_.emplace(id, std::make_unique<Pool<T>>()).first;
        }
        return static_cast<Pool<T>*>(it->second.get())->data;
    }

    inline static size_t next_id_ = 0;

    std::unordered_map<size_t, std::unique_ptr<PoolBase>> pools_;
    std::vector<Mask> masks_;
    Entity next_ = 0;
};

} // namespace ecs
