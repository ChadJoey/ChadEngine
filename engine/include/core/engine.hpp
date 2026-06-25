#pragma once

namespace ChadEngine {

    class Device;
    class ECS;
    class SceneManager;

    enum class EngineState { Editor, Playing, Paused };

    class EngineClass {
    public:
        void Initialize(int width = 1280, int height = 720, const char* title = "Engine");
        void Shutdown();
        void Run();
        void RunFrame(float dt);

        Device& GetDevice() const { return *m_device; }
        ECS& GetECS()    const { return *m_ecs; }
        SceneManager& GetSceneManager() { return *m_scenemanager; }

        EngineState GetState()     const { return m_state; }
        void        SetState(EngineState s) { m_state = s; }
        float       GetDeltaTime() const { return m_deltaTime; }

    private:
        Device* m_device = nullptr;
        ECS* m_ecs = nullptr;
        SceneManager* m_scenemanager;
        float   m_deltaTime = 0.f;
        EngineState m_state = EngineState::Editor;
    };

    extern EngineClass Engine;

} // namespace engine