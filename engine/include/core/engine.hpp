#pragma once
#include "core/ecs.hpp"

namespace ChadEngine {

    class EngineClass {
    public:
        void Initialize();
        void Shutdown();
        void Run();

        ECS& GetECS() { return *m_ecs; }

    private:
        void RunFrame(float dt);
        ECS* m_ecs = nullptr;
    };

    extern EngineClass Engine;

} // namespace engine