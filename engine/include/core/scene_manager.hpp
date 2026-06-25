#pragma once
#include <string>
#include <functional>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "core/types.hpp"

namespace ChadEngine
{
    class SceneManager
    {
    public:

        struct ComponentSerializer
        {
            std::function<void(Entity, nlohmann::json&)> save;
            std::function<void(Entity, const nlohmann::json&)> load;
        };

        void RegisterComponent(const std::string& name, ComponentSerializer serializer);

        void NewScene(const std::string& name);
        void SaveScene(const std::string& path = "");
        void LoadScene(const std::string& path);
        void UnloadScene();

        bool HasScene() const { return !m_currentScene.empty(); }
        const std::string& GetCurrentSceneName() const { return m_currentScene; }


    private:
        std::string m_currentScene;
        std::string m_currentPath;
        std::unordered_map<std::string, ComponentSerializer> m_serializers;

    };
}
