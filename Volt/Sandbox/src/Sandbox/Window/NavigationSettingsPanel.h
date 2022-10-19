#pragma once

#include "Sandbox/Window/EditorWindow.h"

#include "Volt/AI/NavMesh/NavMeshData.h"
#include "Volt/Asset/Mesh/Mesh.h"
#include "Volt/Scene/Scene.h"

enum class PanelView
{
	WORLD,
	OBJECT
};

class NavigationSettingsPanel : public EditorWindow
{
public:
	NavigationSettingsPanel(Ref<Volt::Scene>& currentScene) 
		: EditorWindow("Navigation"), myCurrentScene(currentScene) {}
	void UpdateMainContent() override;

	void Bake();
	Volt::NavMesh CreateNavMesh();

	void DrawVolumeBounds() const;

	void PrintInfo() const;
	void DebugNavMesh();

private:
	PanelView myCurrentView = PanelView::WORLD;

	float myAgentRadius = 50.f;
	float myAgentHeight = 200.f;
	float mySlopeAngle = 45.f;
	float myStepHeight = 0.4f;

	std::vector<Volt::Vertex> myVertices;
	std::vector<uint32_t> myIndices;

	std::unordered_map<uint32_t, bool> myNonWalkableObjects;
	Ref<Volt::Scene>& myCurrentScene;
};