#include "core/device.hpp"
#include "raylib.h"

namespace ChadEngine {

void Device::Init(int width, int height, const char* title) {
    m_width = width;
    m_height = height;
    SetConfigFlags(FLAG_VSYNC_HINT); // must come before InitWindow
    InitWindow(width, height, title);
    SetTargetFPS(60);
}

void Device::Shutdown() {
    CloseWindow();
}

void Device::BeginFrame() {
    BeginDrawing();
    ClearBackground({ 30, 30, 30, 255 });
}

void Device::EndFrame() {
    EndDrawing();
}

bool Device::ShouldClose() const {
    return WindowShouldClose();
}

void Device::EnableGameTexture() {
    m_useGameTexture = true;
    m_gameTexture = LoadRenderTexture(m_width, m_height);
}


void Device::BeginGameRender() {
    if (!m_useGameTexture) return;
    BeginTextureMode(m_gameTexture);
    ClearBackground({ 50, 50, 50, 255 });
}

void Device::EndGameRender() {
    if (!m_useGameTexture) return;
    EndTextureMode();
}

} // namespace engine