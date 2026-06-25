#pragma once
#include "ImReflect.hpp"
#include <glm/glm.hpp>
#include <imgui.h>


namespace glm {
	inline void tag_invoke(ImReflect::ImInput_t, const char* label, vec3& v, ImReflect::ImSettings& s, ImReflect::ImResponse& r) {
		auto& res = r.get<vec3>();
		if (ImGui::DragFloat3(label, &v.x, 0.01f)) res.changed();
		ImReflect::Detail::check_input_states(res);
	}

    inline void tag_invoke(ImReflect::ImInput_t, const char* label, vec2& v,
        ImReflect::ImSettings& s, ImReflect::ImResponse& r) {
        auto& res = r.get<vec2>();
        if (ImGui::DragFloat2(label, &v.x, 0.01f)) res.changed();
        ImReflect::Detail::check_input_states(res);
    }

    inline void tag_invoke(ImReflect::ImInput_t, const char* label, vec4& v,
        ImReflect::ImSettings& s, ImReflect::ImResponse& r) {
        auto& res = r.get<vec4>();
        if (ImGui::DragFloat4(label, &v.x, 0.01f)) res.changed();
        ImReflect::Detail::check_input_states(res);
    }

}