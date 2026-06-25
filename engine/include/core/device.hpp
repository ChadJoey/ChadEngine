#pragma once

namespace ChadEngine {

    class Device {
    public:
        void Init(int width, int height, const char* title);
        void Shutdown();
        void BeginFrame();
        void EndFrame();
        bool ShouldClose() const;

        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }

    private:
        int m_width = 0;
        int m_height = 0;
    };

} // namespace engine