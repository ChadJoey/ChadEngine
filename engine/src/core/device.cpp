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
    ClearBackground(RAYWHITE);
}

void Device::EndFrame() {
    EndDrawing();
}

bool Device::ShouldClose() const {
    return WindowShouldClose();
}

} // namespace engine