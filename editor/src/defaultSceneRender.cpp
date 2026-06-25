#include "defaultSceneRender.hpp"
#include "core/engine.hpp"
#include "core/ecs.hpp"
#include "core/transform.hpp"
#include "raylib.h"

namespace ChadEngine {

    DefaultSceneRenderer::DefaultSceneRenderer() {
        Title = "SceneRenderer";
        IsEditorSystem = false;
    }

    void DefaultSceneRenderer::Render() {
        Camera3D camera = {};
        camera.position = { m_cameraPos.x, m_cameraPos.y, m_cameraPos.z };
        camera.target = { m_cameraTarget.x, m_cameraTarget.y, m_cameraTarget.z };
        camera.up = { 0.f, 1.f, 0.f };
        camera.fovy = m_fovy;
        camera.projection = CAMERA_PERSPECTIVE;

        BeginMode3D(camera);
        DrawGrid(20, 1.f);

        auto view = Engine.GetECS().View<Transform>();
        for (auto entity : view) {
            auto& t = Engine.GetECS().Get<Transform>(entity);
            auto  pos = t.GetWorldPosition();
            auto  sc = t.GetScale();
            DrawCube({ pos.x, pos.y, pos.z }, sc.x, sc.y, sc.z, BLUE);
            DrawCubeWires({ pos.x, pos.y, pos.z }, sc.x, sc.y, sc.z, DARKBLUE);
        }

        EndMode3D();
    }

} // namespace ChadEngine