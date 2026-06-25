#pragma once
#include "core/system.hpp"
#include <glm/glm.hpp>

namespace ChadEngine {

    class DefaultSceneRenderer : public System {
    public:
        DefaultSceneRenderer();
        void Render() override;

    private:
        glm::vec3 m_cameraPos{ 10.f, 10.f, 10.f };
        glm::vec3 m_cameraTarget{ 0.f,  0.f,  0.f };
        float     m_fovy = 60.f;
    };

} // namespace ChadEngine