#include "core/engine.hpp"
#include "core/ecs.hpp"
#include "core/system.hpp"
#include "core/transform.hpp"
#include "raylib.h"

class TestSystem : public ChadEngine::System {
public:
    TestSystem() { Title = "Test"; Priority = 0; }

    void Render() override {
        DrawText("Engine works!", 400, 300, 20, DARKBLUE);
        DrawFPS(10, 10);
    }
};

int main() {
    ChadEngine::Engine.Initialize(1280, 720, "ChadEngine");

    ChadEngine::Engine.GetECS().CreateSystem<TestSystem>();

    auto e = ChadEngine::Engine.GetECS().CreateEntity();
    ChadEngine::Engine.GetECS().CreateComponent<ChadEngine::Transform>(e);
    ChadEngine::Engine.GetECS().Get<ChadEngine::Transform>(e).Name = "TestEntity";

    ChadEngine::Engine.Run();
    ChadEngine::Engine.Shutdown();
    return 0;
}