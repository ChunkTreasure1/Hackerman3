#pragma once

#include "Sandbox/Utility/Helpers.h"
#include "Sandbox/VersionControl/VersionControl.h"

#include <Volt/Scene/Entity.h>
#include <Volt/Core/Layer/Layer.h>
#include <Volt/Events/ApplicationEvent.h>
#include <Volt/Events/KeyEvent.h>
#include <Volt/Utility/DLLHandler.h>

#include <Volt/Rendering/RenderPass.h>

#include <Game/Game.h>

#include <imgui.h>
#include <ImGuizmo.h>

namespace Volt
{
	class SceneRenderer;
	class NavigationsSystem;
	class Scene;
	class Mesh;
	class Framebuffer;
	class Shader;
	class ConstantBuffer;
}

enum class SceneState
{
	Edit,
	Play,
	Pause,
	Simulating
};

struct EditorSettings
{
	VersionControlSettings versionControlSettings;
};

class ViewportPanel;
class EditorWindow;
class EditorCameraController;

class Sandbox : public Volt::Layer
{
public:
	Sandbox();
	~Sandbox() override;

	void OnAttach() override;
	void OnDetach() override;

	void OnEvent(Volt::Event& e) override;

	void OnScenePlay();
	void OnSceneStop();

	void OnSimulationStart();
	void OnSimulationStop();

	inline static Sandbox& Get() { return *myInstance; }

	Ref<Volt::SceneRenderer>& GetSceneRenderer() { return mySceneRenderer; }
	
	void NewScene();
	void OpenScene();
	void SaveScene();
	void TransitionToNewScene();


private:
	struct SaveSceneAsData
	{
		std::string name = "New Level";
		std::filesystem::path destinationPath = "Assets/Levels/";
	} mySaveSceneData;

	void ExecuteUndo();
	void SaveSceneAs();

	void InstallMayaTools();
	void SetupRenderCallbacks();
	void SetupEditorRenderPasses();

	bool OnUpdateEvent(Volt::AppUpdateEvent& e);
	bool OnImGuiUpdateEvent(Volt::AppImGuiUpdateEvent& e);
	bool OnRenderEvent(Volt::AppRenderEvent& e);
	bool OnKeyPressedEvent(Volt::KeyPressedEvent& e);
	bool OnViewportResizeEvent(Volt::ViewportResizeEvent& e);
	bool OnSceneLoadedEvent(Volt::OnSceneLoadedEvent& e);
	bool LoadScene(Volt::OnSceneTransitionEvent& e);

	/////ImGui/////
	void UpdateDockSpace();
	void SaveSceneAsModal();
	///////////////

	std::vector<Ref<EditorWindow>> myEditorWindows;
	Ref<EditorCameraController> myEditorCameraController;

	Ref<Volt::SceneRenderer> mySceneRenderer;
	Ref<Volt::Shader> myGizmoShader;
	Ref<Volt::Shader> myGridShader;

	Ref<Volt::Scene> myRuntimeScene;
	Ref<Volt::Scene> myIntermediateScene;

	/////Outline/////
	Volt::RenderPass mySelectedGeometryPass;
	Volt::RenderPass myJumpFloodInitPass;
	Volt::RenderPass myJumpFloodCompositePass;

	Volt::RenderPass myJumpFloodPass[2];
	Ref<Volt::ConstantBuffer> myJumpFloodPassBuffer;
	/////////////////

	Ref<Game> myGame;

	EditorSettings mySettings;
	SceneState mySceneState = SceneState::Edit;

	Ref<Volt::NavigationsSystem> myNavigationsSystem;
	Ref<ViewportPanel> myViewportPanel;

	ImGuizmo::MODE myGizmoMode = ImGuizmo::MODE::WORLD;
	gem::vec2ui myViewportSize = { 1280, 720 };
	gem::vec2ui myViewportPosition = { 0, 0 };

	bool myOpenShouldSaveScenePopup = false;
	bool myShouldResetLayout = false;

	Ref<Volt::Scene> myStoredScene;
	bool myShouldLoadNewScene = false;

	inline static Sandbox* myInstance = nullptr;
};