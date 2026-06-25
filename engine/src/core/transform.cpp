#include "core/transform.hpp"
#include "core/engine.hpp"
#include "core/ecs.hpp"
#include <cassert>
#include <glm/gtx/quaternion.hpp>

namespace ChadEngine {

    void Transform::SetParent(Entity parent) {
        auto& registry = Engine.GetECS().Registry;
        assert(parent == entt::null || registry.valid(parent));

        const auto& storage = registry.storage<Transform>();
        auto thisEntity = entt::to_entity(storage, *this);

        if (m_parent != entt::null && registry.valid(m_parent))
            registry.get<Transform>(m_parent).RemoveChild(thisEntity);

        if (parent != entt::null && registry.valid(parent))
            registry.get<Transform>(parent).AddChild(thisEntity);

        m_parent = parent;
        SetMatrixDirty();
    }

    void Transform::AddChild(Entity child) {
        auto& registry = Engine.GetECS().Registry;
        if (m_first == entt::null) {
            m_first = child;
        }
        else {
            auto itr = m_first;
            while (true) {
                auto& t = registry.get<Transform>(itr);
                if (t.m_next == entt::null) { t.m_next = child; break; }
                itr = t.m_next;
            }
        }
    }

    void Transform::RemoveChild(Entity child) {
        auto& registry = Engine.GetECS().Registry;
        if (m_first == child) {
            m_first = registry.get<Transform>(m_first).m_next;
        }
        else {
            auto itr = m_first;
            while (itr != entt::null) {
                auto& t = registry.get<Transform>(itr);
                if (t.m_next == child) {
                    t.m_next = registry.get<Transform>(t.m_next).m_next;
                    break;
                }
                itr = t.m_next;
            }
        }
    }

    void Transform::SetMatrixDirty() {
        m_worldMatrixDirty = true;
        for (auto child : *this)
            Engine.GetECS().Registry.get<Transform>(child).SetMatrixDirty();
    }

    const glm::mat4& Transform::World() {
        if (m_worldMatrixDirty) {
            auto t = glm::translate(glm::mat4(1.f), m_translation);
            auto r = glm::toMat4(m_rotation);
            auto s = glm::scale(glm::mat4(1.f), m_scale);
            if (m_parent == entt::null) {
                m_worldMatrix = t * r * s;
            }
            else {
                auto& registry = Engine.GetECS().Registry;
                assert(registry.valid(m_parent));
                m_worldMatrix = registry.get<Transform>(m_parent).World() * t * r * s;
            }
            m_worldMatrixDirty = false;
        }
        return m_worldMatrix;
    }

    Transform::Iterator& Transform::Iterator::operator++() {
        auto& t = Engine.GetECS().Registry.get<Transform>(m_current);
        m_current = t.m_next;
        return *this;
    }

} // namespace engine