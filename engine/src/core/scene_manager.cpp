#include "core/scene_manager.hpp"
#include "core/engine.hpp"
#include "core/ecs.hpp"
#include <fstream>

namespace ChadEngine {


void SceneManager::RegisterComponent(const std::string& name, ComponentSerializer serializer)
{
	m_serializers[name] = serializer;
}



void SceneManager::SaveScene(const std::string& path)
{
	const std::string savePath = path.empty() ? m_currentPath : path;
	if (savePath.empty()) return;

	nlohmann::json sceneJson;
	sceneJson["name"] = m_currentScene;
	sceneJson["entities"] = nlohmann::json::array();

	auto& registry = Engine.GetECS().Registry;

	
	for (auto e : Engine.GetECS().Registry.storage<entt::entity>()) {
		nlohmann::json entityJson;
		entityJson["id"] = static_cast<uint32_t>(e);

		for (auto& [name, serializer] : m_serializers) {
			nlohmann::json componentJson;
			serializer.save(e, componentJson);
			if (!componentJson.is_null())
			{
				entityJson["components"][name] = componentJson;
			}
		}

		sceneJson["entities"].push_back(entityJson);
	}
	std::ofstream file(savePath);
	file << sceneJson.dump(4);
}

void SceneManager::LoadScene(const std::string& path)
{
	std::ifstream file(path);
	if (!file.is_open()) return;

	UnloadScene();

	const auto sceneJson = nlohmann::json::parse(file);
	m_currentScene = sceneJson.value("name", "unnamed");
	m_currentPath = path;

	for (const auto& entityJson : sceneJson["entities"])
	{
		Entity e = Engine.GetECS().CreateEntity();

		if (!entityJson.contains("components")) continue;

		for (auto& [name, serializer] : m_serializers)
		{
			if (entityJson["components"].contains(name))
			{
				serializer.load(e, entityJson["components"][name]);
			}

		}
	}
}


void SceneManager::UnloadScene()
{
	auto& registry = Engine.GetECS().Registry;
	Engine.GetECS().Registry.clear();
	m_currentScene.clear();
	m_currentPath.clear();
}


void SceneManager::NewScene(const std::string& name)
{
	UnloadScene();
	m_currentScene = name;
	m_currentPath = name +".json";
}

}