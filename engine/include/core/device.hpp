#pragma once
#include <raylib.h>

namespace ChadEngine {

    class Device {
    public:
        void Init(int width, int height, const char* title);
        void Shutdown();
        void BeginFrame();
        void EndFrame();
        bool ShouldClose() const;
        void BeginGameRender();
        void EndGameRender();

        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }

        void EnableGameTexture();
        const RenderTexture2D& GetGameTexture() const { return m_gameTexture; }


    private:
        int m_width = 0;
        int m_height = 0;
        bool m_useGameTexture = false;
        RenderTexture2D m_gameTexture = {};
    };

} // namespace engine