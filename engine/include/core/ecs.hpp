#pragma once
#include <memory>
#include <vector>
#include <algorithm>
#include <cassert>
#include <entt/entt.hpp>
#include "core/system.hpp"

namespace ChadEngine {

    using Entity = entt::entity;

    class ECS {
    public:
        entt::registry Registry;

        Entity CreateEntity() {
            return Registry.create();
        }

        void DeleteEntity(Entity e) {
            if (Registry.valid(e))
                Registry.emplace_or_replace<DeleteTag>(e);
        }

        void FlushDeleted() {
            auto view = Registry.view<DeleteTag>();
            Registry.destroy(view.begin(), view.end());
        }

        template<typename T, typename... Args>
        T& CreateComponent(Entity entity, Args&&... args) {
            return Registry.get_or_emplace<T>(entity, std::forward<Args>(args)...);
        }

        template<typename... T>
        void RemoveComponent(Entity entity) {
            Registry.remove<T...>(entity);
        }

        template<typename T>
        T* TryGet(Entity entity) {
            return Registry.try_get<T>(entity);
        }

        template<typename... T>
        decltype(auto) Get(Entity entity) {
            return Registry.get<T...>(entity);
        }

        template<typename... T>
        bool Has(Entity entity) const {
            return Registry.all_of<T...>(entity);
        }

        template<typename... Components>
        auto View() {
            return Registry.view<Components...>();
        }

        template<typename T, typename... Args>
        T& CreateSystem(Args&&... args) {
            auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
            T& ref = *ptr;
            m_systems.push_back(std::move(ptr));
            std::sort(m_systems.begin(), m_systems.end(),
                [](const std::unique_ptr<System>& a, const std::unique_ptr<System>& b) {
                    return a->Priority > b->Priority;
                });
            return ref;
        }

        template<typename T>
        T& GetSystem() {
            for (auto& s : m_systems)
                if (T* found = dynamic_cast<T*>(s.get())) return *found;
            assert(false && "System not found");
            static T dummy; return dummy;
        }

        template<typename T>
        T* TryGetSystem() {
            for (auto& s : m_systems)
                if (T* found = dynamic_cast<T*>(s.get())) return found;
            return nullptr;
        }

        void UpdateSystems(float dt) {
            for (auto& s : m_systems) s->Update(dt);
        }

        void RenderSystems() {
            for (auto& s : m_systems) s->Render();
        }

        std::vector<std::unique_ptr<System>> m_systems;

    private:
        struct DeleteTag {};
    };

} // namespace engine