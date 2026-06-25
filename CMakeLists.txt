cmake_minimum_required(VERSION 3.20)
project(MyEngine)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Editor)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# suppress FetchContent_Populate deprecation warning on CMake 3.30+
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.30")
    cmake_policy(SET CMP0169 OLD)
endif()

include(FetchContent)

# ---- Raylib (master, avoids 6.0 cmake bug) ----
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        master
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(raylib)

# ---- ImGui (no CMakeLists, built manually) ----
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        docking
    GIT_SHALLOW    TRUE
)
FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
endif()
add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp

)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/misc/cpp)
add_library(imgui::imgui ALIAS imgui)

# ---- rlImGui (built manually to avoid its find_package calls) ----
FetchContent_Declare(
    rlimgui_src
    GIT_REPOSITORY https://github.com/raylib-extras/rlImGui.git
    GIT_TAG        main
    GIT_SHALLOW    TRUE
)
FetchContent_GetProperties(rlimgui_src)
if(NOT rlimgui_src_POPULATED)
    FetchContent_Populate(rlimgui_src)
endif()
add_library(rlimgui STATIC
    ${rlimgui_src_SOURCE_DIR}/rlImGui.cpp
)
target_include_directories(rlimgui PUBLIC ${rlimgui_src_SOURCE_DIR})
target_compile_options(rlimgui PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/W0>)
target_link_libraries(rlimgui PUBLIC raylib imgui::imgui)

# ---- ImReflect (single header) ----
FetchContent_Declare(
    imreflect_src
    GIT_REPOSITORY https://github.com/Sven-vh/ImReflect.git
    GIT_TAG        main
    GIT_SHALLOW    TRUE
)
FetchContent_GetProperties(imreflect_src)
if(NOT imreflect_src_POPULATED)
    FetchContent_Populate(imreflect_src)
endif()
add_library(imreflect INTERFACE)
target_include_directories(imreflect INTERFACE ${imreflect_src_SOURCE_DIR})
target_link_libraries(imreflect INTERFACE imgui::imgui)

# ---- CDT (header only) ----
FetchContent_Declare(
    cdt_src
    GIT_REPOSITORY https://github.com/artem-ogre/CDT.git
    GIT_TAG        1.4.1
    GIT_SHALLOW    TRUE
)
FetchContent_GetProperties(cdt_src)
if(NOT cdt_src_POPULATED)
    FetchContent_Populate(cdt_src)
endif()
add_library(cdt_headers INTERFACE)
target_include_directories(cdt_headers INTERFACE ${cdt_src_SOURCE_DIR}/CDT/include)

# ---- stb (header only) ----
FetchContent_Declare(
    stb_src
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        master
    GIT_SHALLOW    TRUE
)
FetchContent_GetProperties(stb_src)
if(NOT stb_src_POPULATED)
    FetchContent_Populate(stb_src)
endif()
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${stb_src_SOURCE_DIR})

# ---- EnTT ----
FetchContent_Declare(
    entt
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG        v3.13.2
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(entt)

# ---- GLM ----
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(glm)

# ---- nlohmann JSON ----
set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

# ---- Box2D ----
set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BOX2D_BUILD_TESTBED    OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    box2d
    GIT_REPOSITORY https://github.com/erincatto/box2d.git
    GIT_TAG        v2.4.1
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(box2d)

# ---- ReactPhysics3D ----
FetchContent_Declare(
    reactphysics3d
    GIT_REPOSITORY https://github.com/DanielChappuis/reactphysics3d.git
    GIT_TAG        v0.10.2
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(reactphysics3d)

# ---- RecastNavigation ----
set(RECASTNAVIGATION_DEMO     OFF CACHE BOOL "" FORCE)
set(RECASTNAVIGATION_TESTS    OFF CACHE BOOL "" FORCE)
set(RECASTNAVIGATION_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    recastnavigation
    GIT_REPOSITORY https://github.com/recastnavigation/recastnavigation.git
    GIT_TAG        v1.6.0
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(recastnavigation)
# ---- EngineLib ----
file(GLOB_RECURSE ENGINE_SOURCES
    CONFIGURE_DEPENDS
    "engine/src/*.cpp"
)

add_library(EngineLib STATIC ${ENGINE_SOURCES})
target_include_directories(EngineLib PUBLIC engine/include)
target_compile_definitions(EngineLib PUBLIC
    GLM_ENABLE_EXPERIMENTAL
)
target_link_libraries(EngineLib PUBLIC
    rlimgui
    imreflect
    cdt_headers
    stb
    EnTT::EnTT
    glm::glm
    nlohmann_json::nlohmann_json
    box2d
    reactphysics3d
    Recast
    Detour
    DetourCrowd
)




# ---- Game ----
file(GLOB_RECURSE GAME_SOURCES
    CONFIGURE_DEPENDS
    "game/src/*.cpp"
)

add_executable(Game ${GAME_SOURCES})
target_link_libraries(Game PRIVATE EngineLib)

# ---- Folder organization ----
set_target_properties(
    imgui glm EnTT box2d reactphysics3d
    Recast Detour DetourCrowd
    PROPERTIES FOLDER "deps"
)


file(GLOB_RECURSE EDITOR_SOURCES
    CONFIGURE_DEPENDS
    "editor/*.cpp"
)

add_executable(Editor ${EDITOR_SOURCES})
target_include_directories(Editor PRIVATE editor/include)
target_link_libraries(Editor PRIVATE EngineLib)

function(add_project PROJECT_NAME)
    file(GLOB_RECURSE PROJECT_SOURCES
        CONFIGURE_DEPENDS
        "projects/${PROJECT_NAME}/*.cpp"
    )
    add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
    target_link_libraries(${PROJECT_NAME} PRIVATE EngineLib)
endfunction()

add_project(EngineTest)