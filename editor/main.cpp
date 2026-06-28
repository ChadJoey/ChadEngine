#include "core/engine.hpp"
#include "core/ecs.hpp"
#include "core/transform.hpp"
#include "core/component_registry.hpp"
#include "core/glm_json.hpp"
#include "include/editor_layer.hpp"
#include "include/defaultSceneRender.hpp"
#include "include/imreflect_glm.hpp"
#include "ImReflect.hpp"
#include <glm/gtx/euler_angles.hpp>

using ChadEngine::Engine;
using ChadEngine::Entity;
using ChadEngine::EditorLayer;
using ChadEngine::DefaultSceneRenderer;

struct TransformView {
    glm::vec3 position{ 0.f };
    glm::vec3 rotation{ 0.f };
    glm::vec3 scale{ 1.f };
};
IMGUI_REFLECT(TransformView, position, rotation, scale)

int main() {
    Engine.Initialize(1280, 720, "ChadEngine Editor");

    // Register Transform with inspector AND save/load in one place
    Engine.GetComponentRegistry().Register({
        .name = "Transform",

        .hasComponent = [](Entity e) {
            return Engine.GetECS().Has<ChadEngine::Transform>(e);
        },

        .inspect = [](Entity e) {
            auto& t = Engine.GetECS().Get<ChadEngine::Transform>(e);
            TransformView view;
            view.position = t.GetTranslation();
            view.rotation = glm::degrees(glm::eulerAngles(t.GetRotation()));
            view.scale = t.GetScale();
            ImReflect::Input("##transform", view);
            t.SetTranslation(view.position);
            t.SetRotation(glm::quat(glm::radians(view.rotation)));
            t.SetScale(view.scale);
        },

        .save = [](Entity e, nlohmann::json& j) {
            auto* t = Engine.GetECS().TryGet<ChadEngine::Transform>(e);
            if (!t) return;
            auto pos = t->GetTranslation();
            auto rot = glm::degrees(glm::eulerAngles(t->GetRotation()));
            auto sc = t->GetScale();
            j["name"] = t->Name;
            j["position"] = { pos.x, pos.y, pos.z };
            j["rotation"] = { rot.x, rot.y, rot.z };
            j["scale"] = { sc.x,  sc.y,  sc.z  };
        },

        .load = [](Entity e, const nlohmann::json& j) {
            auto& t = Engine.GetECS().CreateComponent<ChadEngine::Transform>(e);
            t.Name = j.value("name", "");
            glm::vec3 pos = { j["position"][0], j["position"][1], j["position"][2] };
            glm::vec3 rot = { j["rotation"][0], j["rotation"][1], j["rotation"][2] };
            glm::vec3 sc = { j["scale"][0],    j["scale"][1],    j["scale"][2]    };
            t.SetTranslation(pos);
            t.SetRotation(glm::quat(glm::radians(rot)));
            t.SetScale(sc);
        }
        });

    Engine.GetECS().CreateSystem<DefaultSceneRenderer>();
    Engine.GetECS().CreateSystem<EditorLayer>();

    Engine.Run();
    Engine.Shutdown();
    return 0;
}