#include "sbpch.h"
#include "NavigationSettingsPanel.h"
#include "Volt/Asset/AssetManager.h"
#include "Volt/Scene/Entity.h"
#include "Volt/Components/Components.h"

#include "Volt/Rendering/Renderer.h"
#include "Volt/AI/NavMesh/NavMeshGenerator.h"

#include "Volt/AI/Pathfind/AStar.h"

#include <sstream>

std::vector<Wire::EntityId> gTempEnts;

void NavigationSettingsPanel::UpdateMainContent()
{
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.4f, 0.4f, 1.f));
	if (ImGui::Button("WORLD"))
	{
		myCurrentView = PanelView::WORLD;
	}
	ImGui::SameLine();
	if (ImGui::Button("OBJECT"))
	{
		myCurrentView = PanelView::OBJECT;
	}
	ImGui::PopStyleColor();

	if (myCurrentView == PanelView::WORLD)
	{
		ImGui::InputFloat("Agent Radius", &myAgentRadius);
		ImGui::InputFloat("Agent Height", &myAgentHeight);
		ImGui::SliderFloat("Slope Angle", &mySlopeAngle, 0.f, 60.f, "%.0f");
		ImGui::InputFloat("Step Height", &myStepHeight);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.f));
		if (ImGui::Button("Bake"))
		{
			Bake();
			auto result = CreateNavMesh();
			Volt::NavigationsSystem::SetNavMesh(result);
			DebugNavMesh();
		}
		if (ImGui::Button("CLEAN DEBUG"))
		{
			for (auto ent : gTempEnts)
			{
				if (myCurrentScene->GetRegistry().Exists(ent))
				{
					myCurrentScene->GetRegistry().RemoveEntity(ent);
				}
			}
			gTempEnts.clear();
		}
		ImGui::PopStyleColor();

		PrintInfo();
		DrawVolumeBounds();
	}
	else if (myCurrentView == PanelView::OBJECT)
	{
		if (ImGui::Button("Unwalkable All"))
		{
			myCurrentScene->GetRegistry().ForEach<Volt::MeshComponent>([&](Wire::EntityId id, Volt::MeshComponent& meshComp)
				{
					meshComp.walkable = false;
				});
		}

		if (ImGui::Button("Walkable All"))
		{
			myCurrentScene->GetRegistry().ForEach<Volt::MeshComponent>([&](Wire::EntityId id, Volt::MeshComponent& meshComp)
				{
					meshComp.walkable = true;
				});
		}
	}
}

void NavigationSettingsPanel::Bake()
{
	myVertices.clear();
	myIndices.clear();

	std::vector<Volt::Polygon> polygons;

	myCurrentScene->GetRegistry().ForEach<Volt::MeshComponent>([&](Wire::EntityId id, Volt::MeshComponent& meshComp)
		{
			auto navMeshVerts = Volt::NavMeshGenerator::GetNavMeshVertices(Volt::Entity(id, myCurrentScene.get()), mySlopeAngle);
			//auto navMeshVerts = Volt::NavMeshGenerator::FilterNavMeshVertices(vertsInVolume, upVec, mySlopeAngle, (uint32_t)myVertices.size());

			if (!navMeshVerts.first.empty())
			{
				polygons.push_back(Volt::CreatePolygon(navMeshVerts));
				myVertices.insert(myVertices.end(), navMeshVerts.first.begin(), navMeshVerts.first.end());
				//myIndices.insert(myIndices.end(), navMeshVerts.second.begin(), navMeshVerts.second.end()); // ONLY FOR TESTING RN
			}
		});

	auto finalPoly = Volt::MergePolygons(polygons);
	myIndices = Volt::TriangulatePolygon(finalPoly);
}

Volt::NavMesh NavigationSettingsPanel::CreateNavMesh()
{
	Volt::NavMesh navMesh;
	for (const auto& vertex : myVertices)
	{
		navMesh.Vertices.push_back(vertex);
	}
	navMesh.Indices = myIndices;
	navMesh.Cells = Volt::NavMeshGenerator::GenerateNavMeshCells(navMesh);
	
	return navMesh;
}

void NavigationSettingsPanel::DebugNavMesh()
{
	if (!myVertices.empty() && !myIndices.empty())
	{
		auto entity = myCurrentScene->CreateEntity();
		entity.GetComponent<Volt::TagComponent>().tag = "NavMesh";
		entity.AddComponent<Volt::MeshComponent>();

		if (!entity.IsNull())
		{
			auto material = Volt::AssetManager::GetAsset<Volt::Material>("Assets/Meshes/Primitives/Cube.vtmat");

			Ref<Volt::Mesh> navMesh = CreateRef<Volt::Mesh>(myVertices, myIndices, material);
			Volt::AssetManager::Get().SaveAsset(navMesh);

			entity.GetComponent<Volt::MeshComponent>().handle = navMesh->handle;
			entity.GetComponent<Volt::MeshComponent>().walkable = false;
			gTempEnts.push_back(entity.GetId());
		}

		for (uint32_t u = 0; u < myVertices.size(); u++)
		{
			auto point = myCurrentScene->CreateEntity();
			point.GetComponent<Volt::TagComponent>().tag = std::string("Point-") + std::to_string(u);
			point.GetComponent<Volt::TransformComponent>().position = myVertices[u].position;
			gTempEnts.push_back(point.GetId());
		}
	}
}

void NavigationSettingsPanel::DrawVolumeBounds() const
{
	// This is super ugly but prob just temporary xd
	for (auto& volumeEnt : Volt::NavMeshGenerator::GetModifierVolumes(myCurrentScene))
	{
		const auto& transformComp = volumeEnt.GetComponent<Volt::TransformComponent>();

		auto position = volumeEnt.GetWorldTransform()[3];

		std::vector<gem::vec3> vertexPoints;

		// FRONT
		vertexPoints.push_back(gem::vec3(
			VT_WORLD_UNIT * 0.5f,
			VT_WORLD_UNIT * 0.5f,
			-VT_WORLD_UNIT * 0.5f));

		vertexPoints.push_back(gem::vec3(
			VT_WORLD_UNIT * 0.5f,
			-VT_WORLD_UNIT * 0.5f,
			-VT_WORLD_UNIT * 0.5f));

		vertexPoints.push_back(gem::vec3(
			-VT_WORLD_UNIT * 0.5f,
			-VT_WORLD_UNIT * 0.5f,
			-VT_WORLD_UNIT * 0.5f));

		vertexPoints.push_back(gem::vec3(
			-VT_WORLD_UNIT * 0.5f,
			VT_WORLD_UNIT * 0.5f,
			-VT_WORLD_UNIT * 0.5f));

		// BACK
		vertexPoints.push_back(gem::vec3(
			VT_WORLD_UNIT * 0.5f,
			VT_WORLD_UNIT * 0.5f,
			VT_WORLD_UNIT * 0.5f));

		vertexPoints.push_back(gem::vec3(
			VT_WORLD_UNIT * 0.5f,
			-VT_WORLD_UNIT * 0.5f,
			VT_WORLD_UNIT * 0.5f));

		vertexPoints.push_back(gem::vec3(
			-VT_WORLD_UNIT * 0.5f,
			-VT_WORLD_UNIT * 0.5f,
			VT_WORLD_UNIT * 0.5f));

		vertexPoints.push_back(gem::vec3(
			-VT_WORLD_UNIT * 0.5f,
			VT_WORLD_UNIT * 0.5f,
			VT_WORLD_UNIT * 0.5f));

		for (auto& v : vertexPoints)
		{
			v = gem::vec3(transformComp.GetTransform() * gem::vec4(v.x, v.y, v.z, 1.f));
		}

		gem::vec4 color = { 1.f, 0.f, 0.f, 1.f };

		// Front
		Volt::Renderer::SubmitLine(vertexPoints[0], vertexPoints[1], color);
		Volt::Renderer::SubmitLine(vertexPoints[1], vertexPoints[2], color);
		Volt::Renderer::SubmitLine(vertexPoints[2], vertexPoints[3], color);
		Volt::Renderer::SubmitLine(vertexPoints[3], vertexPoints[0], color);

		// Middle
		Volt::Renderer::SubmitLine(vertexPoints[0], vertexPoints[4], color);
		Volt::Renderer::SubmitLine(vertexPoints[1], vertexPoints[5], color);
		Volt::Renderer::SubmitLine(vertexPoints[2], vertexPoints[6], color);
		Volt::Renderer::SubmitLine(vertexPoints[3], vertexPoints[7], color);

		// Back
		Volt::Renderer::SubmitLine(vertexPoints[4], vertexPoints[5], color);
		Volt::Renderer::SubmitLine(vertexPoints[5], vertexPoints[6], color);
		Volt::Renderer::SubmitLine(vertexPoints[6], vertexPoints[7], color);
		Volt::Renderer::SubmitLine(vertexPoints[7], vertexPoints[4], color);
	}
}

void NavigationSettingsPanel::PrintInfo() const
{
	std::stringstream ss2;
	ss2 << "Vertex Count: " << myVertices.size();

	ImGui::Text(ss2.str().c_str());

	std::stringstream ss3;
	ss3 << "Triangle Count: " << myIndices.size() / 3.f;

	ImGui::Text(ss3.str().c_str());
}
