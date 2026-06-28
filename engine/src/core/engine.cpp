#include "core/engine.hpp"
#include "core/device.hpp"
#include "core/component_registry.hpp"
#include "core/project_manager.hpp"
#include "core/device.hpp"
#include "core/ecs.hpp"
#include <chrono>

namespace ChadEngine {

    EngineClass Engine;

    void EngineClass::Initialize(int width, int height, const char* title) {
        m_device = new Device();
        m_device->Init(width, height, title);
        m_ecs = new ECS();
        m_projectManager = new ProjectManager();
        m_projectManager->Init();
        m_componentRegistry = new ComponentRegistry();
        m_componentRegistry->ProcessPending();
        m_state = EngineState::Playing;
    }

    void EngineClass::Shutdown() {
        delete m_componentRegistry; m_componentRegistry = nullptr;
        delete m_projectManager;    m_projectManager = nullptr;
        delete m_ecs;               m_ecs = nullptr;
        m_device->Shutdown();
        delete m_device;            m_device = nullptr;
    }

    void EngineClass::RunFrame(float dt) {
        m_deltaTime = dt;
        m_ecs->FlushDeleted();
        m_ecs->UpdateSystems(dt);

        m_device->BeginFrame();
        m_device->BeginGameRender();
        m_ecs->RenderGameSystems();  
        m_device->EndGameRender();
        m_ecs->RenderEditorSystems(); 
        m_device->EndFrame();
    }

    void EngineClass::Run() {
        auto time = std::chrono::high_resolution_clock::now();
        while (!m_device->ShouldClose()) {
            auto ctime = std::chrono::high_resolution_clock::now();
            float dt = std::chrono::duration<float>(ctime - time).count();
            RunFrame(dt);
            time = ctime;
        }
    }

} // namespace engine