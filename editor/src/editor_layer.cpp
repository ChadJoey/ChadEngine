#include "editor_layer.hpp"
#include "imreflect_glm.hpp"
#include "core/engine.hpp"
#include "core/ecs.hpp"
#include "core/device.hpp"
#include "core/scene_manager.hpp"
#include "core/component_registry.hpp"
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


	void EditorLayer::RegisterComponentInspector(const std::string& name, std::function<bool(Entity)> hasComponent, std::function<void(Entity)> inspect)
	{
		m_inpsectors.push_back({ name, hasComponent, inspect });
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
			if (ImGui::MenuItem("New Scene"))
			{
				Engine.GetSceneManager().NewScene("untitled");
				m_selectedEntity = entt::null;
			}
			if (ImGui::MenuItem("Open Scene..."))
			{
				m_openScenePopup = true;
				m_scenePathInput.clear();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("save scene", "Ctrl+S"))
			{
				Engine.GetSceneManager().SaveScene();
			}
			if (ImGui::MenuItem("Save Scene as..."))
			{
				m_openScenePopup = false;
				Engine.GetSceneManager().SaveScene();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit"))
			{

			}
			ImGui::EndMenu();
		}


		if (ImGui::BeginMenu("Entity"))
		{
			if (ImGui::MenuItem("Create Empty"))
			{
				auto e = Engine.GetECS().CreateEntity();
				Engine.GetECS().CreateComponent<Transform>(e);
				m_selectedEntity = e;
			}
			if (ImGui::MenuItem("Delete Selected", "Del"))
			{
				if (m_selectedEntity != entt::null)
				{
					Engine.GetECS().DeleteEntity(m_selectedEntity);
					m_selectedEntity = entt::null;
				}
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();

		if (m_openScenePopup)
		{
			ImGui::OpenPopup("Open Scene");
			m_openScenePopup = false;
		}

		if (ImGui::BeginPopupModal("Open Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Scene path:");
			ImGui::InputText("##Scenepath", &m_scenePathInput);
			ImGui::Spacing();
			if (ImGui::Button("Open", ImVec2(120, 0)))
			{
				Engine.GetSceneManager().LoadScene(m_scenePathInput);
				m_selectedEntity = entt::null;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120,0)))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

	}


	void EditorLayer::RenderHierarchy()
	{
		ImGui::Begin("Hierarchy");

		if (Engine.GetSceneManager().HasScene())
		{
			ImGui::TextDisabled("Scene: %s", Engine.GetSceneManager().GetCurrentSceneName().c_str());
			ImGui::Separator();
		}
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