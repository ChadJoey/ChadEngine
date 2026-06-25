#pragma once
#include "core/system.hpp"
#include "core/types.hpp"
#include <functional>
#include <vector>
#include <string>
#include <entt/entt.hpp>


namespace ChadEngine {

	class EditorLayer : public System {
	public:
		EditorLayer();
		~EditorLayer() override;

		void Render() override;

		struct ComponentInspector 
		{
			std::string name;
			std::function<bool(Entity)> hasComponent;
			std::function<void(Entity)> inspect;
		};

		void RegisterComponentInspector(const std::string& name, std::function<bool(Entity)> hasComponent, std::function<void(Entity)> inspect);

		Entity GetSelectedEntity() const { return m_selectedEntity; }
		void SetSelectedEntity(Entity e) { m_selectedEntity = e; }


	private:
		void SetupDockSpace();
		void RenderMenuBar();
		void RenderHierarchy();
		void RenderInspector();
		void RenderViewPort();

		void DrawEntityNode(Entity entity);

		bool m_firstFrame = true;
		Entity m_selectedEntity{ entt::null };

		bool m_openScenePopup = false;
		std::string m_scenePathInput;

		std::vector<ComponentInspector> m_inpsectors;
	};


}