#include "core/engine.hpp"
#include "raylib.h"
#include <chrono>

namespace ChadEngine {

    EngineClass Engine;

    void EngineClass::Initialize() {
        InitWindow(1280, 720, "MyEngine");
        SetTargetFPS(60);
        m_ecs = new ECS();
    }

    void EngineClass::Shutdown() {
        delete m_ecs;
        CloseWindow();
    }

    void EngineClass::RunFrame(float dt) {
        m_ecs->UpdateSystems(dt);

        BeginDrawing();
        ClearBackground(DARKBLUE);
        DrawFPS(10, 10);           
        m_ecs->RenderSystems();
        EndDrawing();
    }

    void EngineClass::Run() {
        while (!WindowShouldClose()) {
            RunFrame(GetFrameTime());
        }
    }

} // namespace engine