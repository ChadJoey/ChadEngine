#include "core/engine.hpp"
#include "core/ecs.hpp"
#include "core/transform.hpp"
#include "core/scene_manager.hpp"
#include "include/editor_layer.hpp"
#include "include/imreflect_glm.hpp"
#include "ImReflect.hpp"

#include <glm/gtx/euler_angles.hpp>

using namespace ChadEngine;

// Proxy struct so ImReflect can reflect Transform's data
// (Transform has private members so we mirror them here)
struct TransformView {
    glm::vec3 position{ 0.f };
    glm::vec3 rotation{ 0.f }; // euler degrees
    glm::vec3 scale{ 1.f };
};
IMGUI_REFLECT(TransformView, position, rotation, scale)

int main() {
    Engine.Initialize(1280, 720, "ChadEngine Editor");

    auto& editor = Engine.GetECS().CreateSystem<EditorLayer>();

    // Register Transform inspector
    editor.RegisterComponentInspector("Transform",
        [](Entity e) {
            return Engine.GetECS().Has<Transform>(e);
        },
        [](Entity e) {
            auto& t = Engine.GetECS().Get<Transform>(e);

            TransformView view;
            view.position = t.GetTranslation();
            view.rotation = glm::degrees(glm::eulerAngles(t.GetRotation()));
            view.scale    = t.GetScale();

            ImReflect::Input("##transform", view);

            t.SetTranslation(view.position);
            t.SetRotation(glm::quat(glm::radians(view.rotation)));
            t.SetScale(view.scale);
        }
    );

    Engine.GetSceneManager().NewScene("default");
    Engine.Run();
    Engine.Shutdown();
    return 0;
}