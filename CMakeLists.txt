cmake_minimum_required(VERSION 3.20)
project(MyEngine)
set(CMAKE_CXX_STANDARD 20)

find_package(raylib CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(box2d CONFIG REQUIRED)
find_package(ReactPhysics3D CONFIG REQUIRED)
find_package(behaviortree_cpp CONFIG REQUIRED)
find_package(recastnavigation CONFIG REQUIRED)

add_library(imreflect INTERFACE)
target_include_directories(imreflect SYSTEM INTERFACE external/ImReflect)
target_link_libraries(imreflect INTERFACE imgui::imgui)

add_library(rlimgui STATIC
    external/rlImGui/rlImGui.cpp
    external/rlImGui/imgui_stdlib.cpp
)
target_include_directories(rlimgui SYSTEM PUBLIC thirdparty/rlImGui)
target_compile_options(rlimgui PRIVATE /W0)
target_link_libraries(rlimgui PUBLIC raylib imgui::imgui)

add_executable(MyEngine
    src/main.cpp
)

target_link_libraries(MyEngine PRIVATE
    imreflect
    rlimgui
    raylib
    imgui::imgui
    EnTT::EnTT
    glm::glm
    nlohmann_json::nlohmann_json
    box2d::box2d
    ReactPhysics3D::ReactPhysics3D
    BT::behaviortree_cpp
    RecastNavigation::Recast
    RecastNavigation::Detour
    RecastNavigation::DetourCrowd
)