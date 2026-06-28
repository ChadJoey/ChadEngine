#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "core/types.hpp"

namespace ChadEngine
{

    struct ProjectInfo {
        std::string name;
        std::string projFilePath;
    };


    class ProjectManager
    {
    public:

        void Init();
        void LoadConfig();
        void SaveConfig() const;

        bool CreateProject(const std::string& name);
        bool LoadProject(const std::string& path);
        void SaveProject();
        void CloseProject();

        std::vector<ProjectInfo> ScanProjects() const;

        bool HasProject() const { return !m_projectRoot.empty(); }
        const std::string& GetName() const { return m_projectName; }
        const std::string& GetRoot() const { return m_projectRoot; }
        const std::string& GetBasePath()  const { return m_basePath; }
        void SetBasePath(const std::string& path) { m_basePath = path; }

        std::string ResolvePath(const std::string& relativePath) const;

    private:

        void SaveEntities(nlohmann::json& json);
        void LoadEntities(const nlohmann::json& json);
        void WriteProjectTemplate();
        void EnsureBasePathExists();


        std::string m_projectRoot;
        std::string m_projectName;
        std::string m_basePath;
        std::string m_configPath = "editor_config.json";
    };
}
