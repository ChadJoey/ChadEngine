#pragma once
#include <memory>
#include <vector>
#include <entt/entt.hpp>
#include "core/system.hpp"

namespace ChadEngine {

    class ECS {
    public:
        entt::registry Registry;

        ECS() = default;
        ~ECS() = default;

        entt::entity CreateEntity() {
            return Registry.create();
        }

        void DeleteEntity(entt::entity e) {
            if (Registry.valid(e))
                Registry.destroy(e);
        }

        // Register a system, returns reference to it
        template<typename T, typename... Args>
        T& RegisterSystem(Args&&... args) {
            auto& ptr = m_systems.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
            return static_cast<T&>(*ptr);
        }

        // Shorthand view query
        template<typename... Components>
        auto View() {
            return Registry.view<Components...>();
        }

        void UpdateSystems(float dt) {
            for (auto& s : m_systems) s->Update(dt);
        }

        void RenderSystems() {
            for (auto& s : m_systems) s->Render();
        }

        std::vector<std::unique_ptr<System>> m_systems;
    };

} // namespace engine