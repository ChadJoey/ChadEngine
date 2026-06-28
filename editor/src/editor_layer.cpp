#include "editor_layer.hpp"
#include "imreflect_glm.hpp"
#include "core/engine.hpp"
#include "core/ecs.hpp"
#include "core/device.hpp"
#include "core/component_registry.hpp"
#include "core/project_manager.hpp"
#include "core/transform.hpp"

#include "rlImgui.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "ImReflect.hpp"
#include <entt/entt.hpp>


namespace ChadEngine {
	
	EditorLayer::EditorLayer() {
		Title = "EditorLayer";
		IsEditorSystem = true;

		Engine.GetDevice().EnableGameTexture(); // tell device to render game offscreen

		rlImGuiSetup(true);
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.IniFilename = "editor_layout.ini";

	}

	EditorLayer::~EditorLayer() {
		rlImGuiShutdown();
	}


	void EditorLayer::Render()
	{
		rlImGuiBegin();
		SetupDockSpace();
		RenderHierarchy();
		RenderInspector();
		RenderViewPort();
		rlImGuiEnd();
	}


	void EditorLayer::SetupDockSpace()
	{
		ImGuiViewport* vp = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(vp->Pos);
		ImGui::SetNextWindowSize(vp->Size);
		ImGui::SetNextWindowViewport(vp->ID);

		ImGuiWindowFlags hostFlags =
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
			ImGuiWindowFlags_MenuBar;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
		ImGui::Begin("##EditorRoot", nullptr, hostFlags);
		ImGui::PopStyleVar(3);

		ImGuiID dockId = ImGui::GetID("EditorDockspace");
		ImGui::DockSpace(dockId, ImVec2(0.f, 0.f));

		if (m_firstFrame) {
			m_firstFrame = false;

			ImGui::DockBuilderRemoveNode(dockId);
			ImGui::DockBuilderAddNode(dockId, ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(dockId, vp->Size);

			ImGuiID center = dockId;
			ImGuiID left, right, bottom;

			ImGui::DockBuilderSplitNode(center, ImGuiDir_Left, 0.20f, &left, &center);
			ImGui::DockBuilderSplitNode(center, ImGuiDir_Right, 0.28f, &right, &center);
			ImGui::DockBuilderSplitNode(center, ImGuiDir_Down, 0.25f, &bottom, &center);

			ImGui::DockBuilderDockWindow("Hierarchy", left);
			ImGui::DockBuilderDockWindow("Inspector", right);
			ImGui::DockBuilderDockWindow("Viewport", center);
			ImGui::DockBuilderDockWindow("Console", bottom);

			ImGui::DockBuilderFinish(dockId);
		}

		RenderMenuBar();
		ImGui::End();
	}

	void EditorLayer::RenderMenuBar()
	{
		if (!ImGui::BeginMenuBar()) return;

		if (ImGui::BeginMenu("File"))
		{
			// these just set flags, never open popups directly from inside a menu
			if (ImGui::MenuItem("New Project...")) {
				m_newProjectPopup = true;
				m_projectNameInput.clear();
			}
			if (ImGui::MenuItem("Open Project...")) {
				m_openProjectPopup = true;
				m_selectedProject = -1;
				m_projectFilter.clear();
			}

			ImGui::Separator();

			bool hasProject = Engine.GetProjectManager().HasProject();

			if (ImGui::MenuItem("Save Project", "Ctrl+S", false, hasProject))
				Engine.GetProjectManager().SaveProject();

			if (ImGui::MenuItem("Close Project", nullptr, false, hasProject)) {
				Engine.GetProjectManager().CloseProject();
				m_selectedEntity = entt::null;
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Exit")) {}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Entity"))
		{
			if (ImGui::MenuItem("Create Empty")) {
				auto e = Engine.GetECS().CreateEntity();
				Engine.GetECS().CreateComponent<Transform>(e);
				m_selectedEntity = e;
			}
			if (ImGui::MenuItem("Delete Selected", "Del")) {
				if (m_selectedEntity != entt::null) {
					Engine.GetECS().DeleteEntity(m_selectedEntity);
					m_selectedEntity = entt::null;
				}
			}
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();

		// ---- all popups go here, AFTER EndMenuBar ----

		if (m_newProjectPopup) {
			ImGui::OpenPopup("New Project");
			m_newProjectPopup = false;
		}
		if (ImGui::BeginPopupModal("New Project", nullptr,
			ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Project Name:");
			ImGui::InputText("##projname", &m_projectNameInput);
			ImGui::TextDisabled("Will be created in:");
			ImGui::TextDisabled("%s/%s",
				Engine.GetProjectManager().GetBasePath().c_str(),
				m_projectNameInput.c_str());
			ImGui::Spacing();
			if (ImGui::Button("Create", ImVec2(120, 0))) {
				if (!m_projectNameInput.empty())
					Engine.GetProjectManager().CreateProject(m_projectNameInput);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0)))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		if (m_openProjectPopup) {
			ImGui::OpenPopup("Open Project");
			m_openProjectPopup = false;
		}
		if (ImGui::BeginPopupModal("Open Project", nullptr,
			ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Projects in: %s",
				Engine.GetProjectManager().GetBasePath().c_str());
			ImGui::Separator();

			ImGui::SetNextItemWidth(400.f);
			ImGui::InputText("Filter", &m_projectFilter);
			ImGui::Spacing();

			auto projects = Engine.GetProjectManager().ScanProjects();

			ImGui::BeginChild("##projectlist", ImVec2(400.f, 300.f),
				true, ImGuiWindowFlags_None);

			if (projects.empty()) {
				ImGui::TextDisabled("No projects found.");
				ImGui::TextDisabled("Create one with File > New Project.");
			}

			for (int i = 0; i < static_cast<int>(projects.size()); i++) {
				auto& proj = projects[i];

				if (!m_projectFilter.empty()) {
					auto nameLC = proj.name;
					auto filterLC = m_projectFilter;
					std::transform(nameLC.begin(), nameLC.end(),
						nameLC.begin(), ::tolower);
					std::transform(filterLC.begin(), filterLC.end(),
						filterLC.begin(), ::tolower);
					if (nameLC.find(filterLC) == std::string::npos) continue;
				}

				bool selected = (m_selectedProject == i);
				ImGui::PushID(i);
				if (ImGui::Selectable("##sel", selected,
					ImGuiSelectableFlags_AllowDoubleClick, ImVec2(0.f, 40.f)))
				{
					m_selectedProject = i;
					if (ImGui::IsMouseDoubleClicked(0)) {
						Engine.GetProjectManager().LoadProject(proj.projFilePath);
						m_selectedEntity = entt::null;
						ImGui::CloseCurrentPopup();
					}
				}

				ImVec2 pos = ImGui::GetItemRectMin();
				ImGui::GetWindowDrawList()->AddText(
					{ pos.x + 8.f, pos.y + 4.f },
					IM_COL32(255, 255, 255, 255),
					proj.name.c_str());
				ImGui::GetWindowDrawList()->AddText(
					{ pos.x + 8.f, pos.y + 22.f },
					IM_COL32(160, 160, 160, 255),
					proj.projFilePath.c_str());

				ImGui::PopID();
			}

			ImGui::EndChild();
			ImGui::Spacing();

			bool canOpen = (m_selectedProject >= 0 &&
				m_selectedProject < static_cast<int>(projects.size()));

			if (!canOpen) ImGui::BeginDisabled();
			if (ImGui::Button("Open", ImVec2(120.f, 0.f))) {
				Engine.GetProjectManager().LoadProject(
					projects[m_selectedProject].projFilePath);
				m_selectedEntity = entt::null;
				ImGui::CloseCurrentPopup();
			}
			if (!canOpen) ImGui::EndDisabled();

			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120.f, 0.f)))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}
	}


	void EditorLayer::RenderHierarchy()
	{
		ImGui::Begin("Hierarchy");

		auto view = Engine.GetECS().View<Transform>();
		for (auto entity : view)
		{
			auto& t = Engine.GetECS().Registry.get<Transform>(entity);
			if (!t.HasParent())
			{
				DrawEntityNode(entity);
			}
		}

		if (ImGui::BeginPopupContextWindow("##HierCtx", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create empty"))
			{
				auto e = Engine.GetECS().CreateEntity();
				Engine.GetECS().CreateComponent<Transform>(e);
				m_selectedEntity = e;
			}
			ImGui::EndPopup();
		}
		ImGui::End();
	}


	void EditorLayer::RenderInspector()
	{
		ImGui::Begin("Inspector");


		if (m_selectedEntity == entt::null || !Engine.GetECS().Registry.valid(m_selectedEntity))
		{
			ImGui::TextDisabled("No entity selected");
			ImGui::End();
			return;
		}


		auto* t = Engine.GetECS().TryGet<Transform>(m_selectedEntity);

		if (t)
		{
			ImGui::SetNextItemWidth(-1);
			ImGui::InputText("##entityname", &t->Name);
			ImGui::Spacing();
		}

		ImGui::TextDisabled("ID: %u", static_cast<uint32_t>(m_selectedEntity));
		ImGui::Separator();


		for (auto& entry : Engine.GetComponentRegistry().GetAll()) {
			if (!entry.hasComponent(m_selectedEntity)) continue;

			bool open = ImGui::CollapsingHeader(
				entry.name.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

			if (open) {
				ImGui::PushID(entry.name.c_str());
				ImGui::Indent(8.f);
				entry.inspect(m_selectedEntity);
				ImGui::Unindent(8.f);
				ImGui::PopID();
			}
		}


		ImGui::End();
	}

	void EditorLayer::RenderViewPort() {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
		ImGui::Begin("Viewport");
		ImGui::PopStyleVar();

		ImVec2 size = ImGui::GetContentRegionAvail();

		const RenderTexture2D& tex = Engine.GetDevice().GetGameTexture();

		// OpenGL textures are flipped on Y, Rectangle with negative height corrects this
		Rectangle src = {
			0.f,
			0.f,
			static_cast<float>(tex.texture.width),
			-static_cast<float>(tex.texture.height)
		};

		rlImGuiImageRect(&tex.texture,
			static_cast<int>(size.x),
			static_cast<int>(size.y),
			src);

		ImGui::End();

		ImGui::Begin("Console");
		ImGui::TextDisabled("Console output will go here");
		ImGui::End();
	}

	void EditorLayer::DrawEntityNode(Entity entity)
	{
		auto& t = Engine.GetECS().Registry.get<Transform>(entity);

		std::string name = t.Name.empty() ? ("Entity " + std::to_string(static_cast<uint32_t>(entity))) : t.Name;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

		if (m_selectedEntity == entity)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		if (!t.HasChildren())
		{
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		}

		bool opened = ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<uint64_t>(entity)), flags, "%s", name.c_str());

		if (ImGui::IsItemClicked())
		{
			m_selectedEntity = entity;
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Rename"))
			{
				//TODO
			}

			if (ImGui::MenuItem("Duplicate"))
			{
				//TODO
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Delete"))
			{
				Engine.GetECS().DeleteEntity(entity);
				if (m_selectedEntity == entity)
				{
					m_selectedEntity = entt::null;
				}
			}
			ImGui::EndPopup();
		}
		if (opened && t.HasChildren())
		{
			for (auto child : t)
			{
				DrawEntityNode(child);
			}
				ImGui::TreePop();
		}


	}

}