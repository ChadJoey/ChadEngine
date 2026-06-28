#include "core/project_manager.hpp"
#include "core/engine.hpp"
#include "core/ecs.hpp"
#include "core/component_registry.hpp"
#include <fstream>
#include <entt/entt.hpp>

namespace ChadEngine {


    void ProjectManager::Init() {
        // default to a projects folder next to wherever the editor is running from
        m_basePath = (std::filesystem::current_path() / "projects").string();
        LoadConfig();
        EnsureBasePathExists();
    }



    void ProjectManager::EnsureBasePathExists() {
        std::filesystem::create_directories(m_basePath);
    }


    void ProjectManager::LoadConfig() {
        std::ifstream file(m_configPath);
        if (!file.is_open()) return;
        auto json = nlohmann::json::parse(file);
        m_basePath = json.value("projectsBasePath", m_basePath);
    }


    std::vector<ProjectInfo> ProjectManager::ScanProjects() const {
        std::vector<ProjectInfo> results;
        if (!std::filesystem::exists(m_basePath)) return results;

        for (const auto& entry : std::filesystem::directory_iterator(m_basePath)) {
            if (!entry.is_directory()) continue;

            // look for a .chadproj file inside this folder
            for (const auto& file : std::filesystem::directory_iterator(entry.path())) {
                if (file.path().extension() == ".chadproj") {
                    results.push_back({
                        entry.path().filename().string(),
                        file.path().string()
                        });
                    break;
                }
            }
        }

        return results;
    }


    void ProjectManager::SaveConfig() const {
        nlohmann::json json;
        json["projectsBasePath"] = m_basePath;
        std::ofstream file(m_configPath);
        file << json.dump(4);
    }

    bool ProjectManager::CreateProject(const std::string& name) {
        namespace fs = std::filesystem;

        fs::path root = fs::path(m_basePath) / name;

        // create standardized folder structure
        fs::create_directories(root / "include");
        fs::create_directories(root / "src");
        fs::create_directories(root / "assets");

        m_projectName = name;
        m_projectRoot = root.string();

        WriteProjectTemplate();
        SaveProject();

        return true;
    }

    void ProjectManager::WriteProjectTemplate() {
        std::string gameCpp = m_projectRoot + "/src/game.cpp";
        if (std::filesystem::exists(gameCpp)) return;

        std::ofstream file(gameCpp);
        file << "// ChadEngine - " << m_projectName << "\n"
            << "// GameSetup() is called by the engine when entering play mode.\n\n"
            << "#include \"core/engine.hpp\"\n\n"
            << "// Add your component headers here\n\n"
            << "void GameSetup() {\n"
            << "    // Register your systems here\n"
            << "}\n";
    }

    bool ProjectManager::LoadProject(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) return false;

        auto json = nlohmann::json::parse(file);
        m_projectName = json.value("name", "Unnamed");
        m_projectRoot = std::filesystem::path(path).parent_path().string();

        Engine.GetECS().Registry.clear();
        if (json.contains("entities"))
            LoadEntities(json);

        return true;
    }

    void ProjectManager::SaveProject() {
        if (m_projectRoot.empty()) return;

        nlohmann::json json;
        json["name"] = m_projectName;
        json["entities"] = nlohmann::json::array();

        SaveEntities(json);

        std::ofstream file(m_projectRoot + "/" + m_projectName + ".chadproj");
        file << json.dump(4);
    }

    void ProjectManager::CloseProject() {
        Engine.GetECS().Registry.clear();
        m_projectRoot.clear();
        m_projectName.clear();
    }

    void ProjectManager::SaveEntities(nlohmann::json& json) {
        for (auto e : Engine.GetECS().Registry.storage<entt::entity>()) {
            nlohmann::json entityJson;
            entityJson["id"] = static_cast<uint32_t>(e);

            for (auto& entry : Engine.GetComponentRegistry().GetAll()) {
                if (!entry.hasComponent(e)) continue;
                nlohmann::json componentJson;
                entry.save(e, componentJson);
                if (!componentJson.is_null())
                    entityJson["components"][entry.name] = componentJson;
            }

            json["entities"].push_back(entityJson);
        }
    }

    void ProjectManager::LoadEntities(const nlohmann::json& json) {
        for (const auto& entityJson : json["entities"]) {
            Entity e = Engine.GetECS().CreateEntity();
            if (!entityJson.contains("components")) continue;

            for (auto& entry : Engine.GetComponentRegistry().GetAll()) {
                if (!entityJson["components"].contains(entry.name)) continue;
                entry.load(e, entityJson["components"][entry.name]);
            }
        }
    }

    std::string ProjectManager::ResolvePath(const std::string& relativePath) const {
        return (std::filesystem::path(m_projectRoot) / relativePath).string();
    }

} // namespace ChadEngine