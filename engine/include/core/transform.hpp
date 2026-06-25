#pragma once
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <entt/entt.hpp>
#include "core/types.hpp"

namespace ChadEngine {
    struct Transform {
        std::string Name;

        Transform() = default;
        Transform(const glm::vec3& translation, const glm::vec3& scale, const glm::quat& rotation)
            : m_translation(translation), m_scale(scale), m_rotation(rotation) {
        }

        const glm::vec3& GetTranslation() const { return m_translation; }
        const glm::vec3& GetScale()       const { return m_scale; }
        const glm::quat& GetRotation()    const { return m_rotation; }
        Entity           GetParent()      const { return m_parent; }
        bool             HasParent()      const { return m_parent != entt::null; }
        bool             HasChildren()    const { return m_first != entt::null; }

        void SetTranslation(const glm::vec3& t) { m_translation = t; SetMatrixDirty(); }
        void SetScale(const glm::vec3& s) { m_scale = s;       SetMatrixDirty(); }
        void SetRotation(const glm::quat& r) { m_rotation = r;    SetMatrixDirty(); }

        void SetParent(Entity parent);
        const glm::mat4& World();

        glm::vec3 GetWorldPosition() { return glm::vec3(World()[3]); }
        glm::vec3 GetForward() { return glm::normalize(glm::vec3(World()[2])); }
        glm::vec3 GetRight() { return glm::normalize(glm::vec3(World()[0])); }
        glm::vec3 GetUp() { return glm::normalize(glm::vec3(World()[1])); }

        class Iterator {
        public:
            Iterator() = default;
            Iterator(Entity e) : m_current(e) {}
            Iterator& operator++();
            bool operator!=(const Iterator& other) { return m_current != other.m_current; }
            Entity operator*() { return m_current; }
        private:
            Entity m_current{ entt::null };
        };

        Iterator begin() const { return { m_first }; }
        static Iterator end() { return {}; }

    private:
        glm::vec3 m_translation{ 0.f };
        glm::vec3 m_scale{ 1.f };
        glm::quat m_rotation{ glm::identity<glm::quat>() };
        glm::mat4 m_worldMatrix{ glm::identity<glm::mat4>() };
        bool m_worldMatrixDirty = true;

        Entity m_parent{ entt::null };
        Entity m_first{ entt::null };
        Entity m_next{ entt::null };

        void AddChild(Entity child);
        void RemoveChild(Entity child);
        void SetMatrixDirty();
    };

} // namespace engine