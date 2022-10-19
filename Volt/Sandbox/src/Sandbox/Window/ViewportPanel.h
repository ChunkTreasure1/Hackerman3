#pragma once
#pragma once

#include "Sandbox/Window/EditorWindow.h"
#include "Sandbox/Window/AssetCommon.h"
#include "Sandbox/Utility/EditorUtilities.h"
#include "Sandbox/Utility/AnimatedIcon.h"

#include "Sandbox/Sandbox.h"

#include <Volt/Events/MouseEvent.h>

#include <gem/gem.h>
#include <Wire/Entity.h>

#include <imgui.h>
#include <ImGuizmo.h>

struct GizmoEvent
{
	//[0] = transform
	//[1] = rotation
	//[2] = scale
	Wire::EntityId myEntityId;
	std::array<gem::vec3, 3> myValue;
};

namespace Volt
{
	class Framebuffer;
	class SceneRenderer;
	class Scene;
}

class EditorCameraController;
class ViewportPanel : public EditorWindow
{
public:
	ViewportPanel(Ref<Volt::SceneRenderer>& sceneRenderer, Ref<Volt::Scene>& editorScene, EditorCameraController* cameraController, ImGuizmo::MODE& gizmoMode, SceneState& aSceneState);

	void UpdateMainContent() override;
	void UpdateContent() override;
	void OnEvent(Volt::Event& e) override;

private:
	bool OnMouseMoved(Volt::MouseMovedEvent& e);
	bool OnMousePressed(Volt::MouseButtonPressedEvent& e);
	bool OnKeyPressedEvent(Volt::KeyPressedEvent& e);
	
	gem::mat4 CalculateAverageTransform();

	Ref<Volt::SceneRenderer>& mySceneRenderer;
	Ref<Volt::Scene>& myEditorScene;

	AnimatedIcon myAnimatedPhysicsIcon;
	EditorCameraController* myEditorCameraController;

	gem::vec2 myPerspectiveBounds[2] = { { 0.f, 0.f }, { 0.f, 0.f } };
	gem::vec2 myViewportSize = { 1280.f, 720.f };

	ImGuizmo::OPERATION myGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	ImGuizmo::MODE& myGizmoMode;
	SceneState& mySceneState;

	bool myIsWorldSpace = true;
	bool mySnapToGrid = false;
	bool mySnapRotation = false;
	bool mySnapScale = false;
	bool myShowGizmos = false;

	const std::vector<float> m_snapToGridValues = { 1.f, 10.f, 25.f, 50.f, 100.f, 200.f, 500.f, 1000.f };
	const std::vector<float> m_snapRotationValues = { 10.f, 30.f, 45.f, 90.f };
	const std::vector<float> m_snapScaleValues = { 0.01f, 0.1f, 0.25f, 0.5f, 1.f };

	float m_gridSnapValue = 50.f;
	float m_rotateSnapValue = 45.f;
	float m_scaleSnapValue = 0.1f;

	gem::vec2 myViewportMouseCoords;
	bool myMidEvent;

	Wire::EntityId myEntityToAddMesh = Wire::NullID;
	std::filesystem::path myMeshToImport;
	MeshImportData myMeshImportData;

	Volt::AssetHandle mySceneToOpen = Volt::Asset::Null();
};

