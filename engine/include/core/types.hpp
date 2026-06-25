#pragma once
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <cstdint>

namespace ChadEngine {

	using Entity = entt::entity;
	using UUID = uint64_t;
	using Vec2 = glm::vec2;
	using Vec3 = glm::vec3;
	using Vec4 = glm::vec4;
	using Mat4 = glm::mat4;
	using Quat = glm::quat;

} // namespace engine