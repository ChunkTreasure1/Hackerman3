#include "sbpch.h"
#include "ViewportPanel.h"

#include "Sandbox/Camera/EditorCameraController.h"
#include "Sandbox/Window/EditorIconLibrary.h"
#include "Sandbox/Utility/SelectionManager.h"
#include "Sandbox/Sandbox.h"

#include <Volt/Asset/Mesh/Mesh.h>
#include <Volt/Asset/Mesh/Material.h>
#include <Volt/Asset/ParticlePreset.h>
#include <Volt/Asset/Prefab.h>
#include <Volt/Components/Components.h>

#include <Volt/Input/Input.h>
#include <Volt/Input/KeyCodes.h>
#include <Volt/Input/MouseButtonCodes.h>

#include <Volt/Rendering/SceneRenderer.h>
#include <Volt/Rendering/Framebuffer.h>
#include <Volt/Rendering/Camera/Camera.h>
#include <Volt/Rendering/Framebuffer.h>
#include <Volt/Rendering/Renderer.h>

#include <Volt/Scene/Entity.h>
#include <Volt/Utility/UIUtility.h>
#include <Volt/Utility/Math.h>

#include <Volt/Utility/StringUtility.h>

#include "Sandbox/EditorCommandStack.h"

ViewportPanel::ViewportPanel(Ref<Volt::SceneRenderer>& sceneRenderer, Ref<Volt::Scene>& editorScene, EditorCameraController* cameraController,
	ImGuizmo::MODE& gizmoMode, SceneState& aSceneState)
	: EditorWindow("Viewport"), mySceneRenderer(sceneRenderer), myEditorCameraController(cameraController), myEditorScene(editorScene),
	myGizmoMode(gizmoMode), mySceneState(aSceneState), myAnimatedPhysicsIcon("Editor/Textures/Icons/Physics/LampPhysicsAnim1.dds", 30)
{
	myIsOpen = true;
	myWindowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;
	myMidEvent = false;
}

void ViewportPanel::UpdateMainContent()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 0.f, 0.f });
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2{ 0.f, 0.f });
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.f, 0.f });

	auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
	auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
	auto viewportOffset = ImGui::GetWindowPos();

	myPerspectiveBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
	myPerspectiveBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

	{
		auto [mx, my] = ImGui::GetMousePos();
		mx -= myPerspectiveBounds[0].x;
		my -= myPerspectiveBounds[0].y;

		gem::vec2 perspectiveSize = myPerspectiveBounds[1] - myPerspectiveBounds[0];

		myViewportMouseCoords = { mx, my };
	}

	ImVec2 viewportSize = ImGui::GetContentRegionAvail();

	if (myViewportSize != (*(gem::vec2*)&viewportSize) && viewportSize.x > 0 && viewportSize.y > 0 && !Volt::Input::IsMouseButtonPressed(VT_MOUSE_BUTTON_LEFT))
	{
		myViewportSize = { viewportSize.x, viewportSize.y };
		mySceneRenderer->Resize((uint32_t)myViewportSize.x, (uint32_t)myViewportSize.y);
		myEditorScene->SetRenderSize((uint32_t)myViewportSize.x, (uint32_t)myViewportSize.y);

		myEditorCameraController->UpdateProjection((uint32_t)myViewportSize.x, (uint32_t)myViewportSize.y);

		Volt::ViewportResizeEvent resizeEvent{ (uint32_t)myPerspectiveBounds[0].x, (uint32_t)myPerspectiveBounds[0].y, (uint32_t)myViewportSize.x, (uint32_t)myViewportSize.y };
		Volt::Application::Get().OnEvent(resizeEvent);
	}

	ImGui::Image(UI::GetTextureID(mySceneRenderer->GetFinalFramebuffer()->GetColorAttachment(0)), viewportSize);
	if (void* ptr = UI::DragDropTarget({ "ASSET_BROWSER_ITEM" }))
	{
		const Volt::AssetHandle handle = *(const Volt::AssetHandle*)ptr;
		const Volt::AssetType type = Volt::AssetManager::Get().GetAssetTypeFromHandle(handle);

		switch (type)
		{
			case Volt::AssetType::Mesh:
			{
				Volt::Entity newEntity = myEditorScene->CreateEntity();

				Ref<ObjectStateCommand> command = CreateRef<ObjectStateCommand>(newEntity, ObjectStateAction::Create);
				EditorCommandStack::GetInstance().PushUndo(command);

				auto& meshComp = newEntity.AddComponent<Volt::MeshComponent>();
				auto mesh = Volt::AssetManager::GetAsset<Volt::Mesh>(handle);
				if (mesh)
				{
					meshComp.handle = mesh->handle;
				}

				newEntity.GetComponent<Volt::TagComponent>().tag = Volt::AssetManager::Get().GetPathFromAssetHandle(handle).stem().string();

				break;
			}

			case Volt::AssetType::MeshSource:
			{
				const std::filesystem::path meshSourcePath = Volt::AssetManager::Get().GetPathFromAssetHandle(handle);
				const std::filesystem::path vtMeshPath = meshSourcePath.parent_path() / (meshSourcePath.stem().string() + ".vtmesh");

				Volt::AssetHandle resultHandle = handle;
				Volt::Entity newEntity = myEditorScene->CreateEntity();

				Ref<ObjectStateCommand> command = CreateRef<ObjectStateCommand>(newEntity, ObjectStateAction::Create);
				EditorCommandStack::GetInstance().PushUndo(command);

				if (FileSystem::Exists(vtMeshPath))
				{
					Ref<Volt::Mesh> meshAsset = Volt::AssetManager::GetAsset<Volt::Mesh>(vtMeshPath);
					if (meshAsset && meshAsset->IsValid())
					{
						resultHandle = meshAsset->handle;
					}

					auto& meshComp = newEntity.AddComponent<Volt::MeshComponent>();
					auto mesh = Volt::AssetManager::GetAsset<Volt::Mesh>(resultHandle);
					if (mesh)
					{
						meshComp.handle = mesh->handle;
					}
				}
				else
				{
					myEntityToAddMesh = newEntity.GetId();

					myMeshImportData = {};
					myMeshToImport = meshSourcePath;
					myMeshImportData.destination = myMeshToImport.parent_path().string() + "\\" + myMeshToImport.stem().string() + ".vtmesh";
					UI::OpenModal("Import Mesh##viewport");
				}

				newEntity.GetComponent<Volt::TagComponent>().tag = meshSourcePath.stem().string();

				break;
			}

			case Volt::AssetType::ParticlePreset:
			{
				Volt::Entity newEntity = myEditorScene->CreateEntity();

				auto& particleEmitter = newEntity.AddComponent<Volt::ParticleEmitterComponent>();
				auto preset = Volt::AssetManager::GetAsset<Volt::ParticlePreset>(handle);
				if (preset)
				{
					particleEmitter.preset = preset->handle;
				}

				newEntity.GetComponent<Volt::TagComponent>().tag = Volt::AssetManager::Get().GetPathFromAssetHandle(handle).stem().string();

				break;
			}

			case Volt::AssetType::Material:
			{
				auto material = Volt::AssetManager::GetAsset<Volt::Material>(handle);
				if (!material || !material->IsValid())
				{
					break;
				}

				gem::vec2 perspectiveSize = myPerspectiveBounds[1] - myPerspectiveBounds[0];

				int32_t mouseX = (int32_t)myViewportMouseCoords.x;
				int32_t mouseY = (int32_t)myViewportMouseCoords.y;

				if (mouseX >= 0 && mouseY >= 0 && mouseX < (int32_t)perspectiveSize.x && mouseY < (int32_t)perspectiveSize.y)
				{
					uint32_t pixelData = mySceneRenderer->GetSelectionFramebuffer()->ReadPixel<uint32_t>(5, mouseX, mouseY);

					if (myEditorScene->GetRegistry().HasComponent<Volt::MeshComponent>(pixelData))
					{
						auto& meshComponent = myEditorScene->GetRegistry().GetComponent<Volt::MeshComponent>(pixelData);
						meshComponent.overrideMaterial = material->handle;
					}
				}

				break;
			}

			case Volt::AssetType::Scene:
			{
				UI::OpenModal("Do you want to save scene?##OpenSceneViewport");
				mySceneToOpen = handle;

				break;
			}

			case Volt::AssetType::Prefab:
			{
				auto prefab = Volt::AssetManager::GetAsset<Volt::Prefab>(handle);
				if (!prefab || !prefab->IsValid())
				{
					break;
				}

				Wire::EntityId id = prefab->Instantiate(myEditorScene->GetRegistry());
				Volt::Entity prefabEntity(id, myEditorScene.get());

				Ref<ObjectStateCommand> command = CreateRef<ObjectStateCommand>(prefabEntity, ObjectStateAction::Create);
				EditorCommandStack::GetInstance().PushUndo(command);

				break;
			}
		}
	}

	// Gizmo
	{
		static gem::mat4 averageTransform = gem::mat4(1.f);
		static gem::mat4 averageStartTransform = gem::mat4(1.f);

		static bool hasDuplicated = false;
		static bool isUsing = false;

		if (SelectionManager::IsAnySelected() && mySceneState != SceneState::Play)
		{
			averageTransform = CalculateAverageTransform();
			bool snap = Volt::Input::IsKeyDown(VT_KEY_LEFT_CONTROL);
			const bool duplicate = Volt::Input::IsKeyDown(VT_KEY_LEFT_ALT);

			float snapValue = 0.5f;
			if (myGizmoOperation == ImGuizmo::ROTATE)
			{
				snap = mySnapRotation && snap ? false : mySnapRotation && !snap ? true : snap;
				snapValue = m_rotateSnapValue;
			}
			else if (myGizmoOperation == ImGuizmo::SCALE)
			{
				snap = mySnapScale && snap ? false : mySnapScale && !snap ? true : snap;
				snapValue = m_scaleSnapValue;
			}
			else if (myGizmoOperation == ImGuizmo::TRANSLATE)
			{
				snap = mySnapToGrid && snap ? false : mySnapToGrid && !snap ? true : snap;
				snapValue = m_gridSnapValue;
			}

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::MODE gizmoMode = myIsWorldSpace ? ImGuizmo::WORLD : ImGuizmo::LOCAL;

			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(myPerspectiveBounds[0].x, myPerspectiveBounds[0].y, myPerspectiveBounds[1].x - myPerspectiveBounds[0].x, myPerspectiveBounds[1].y - myPerspectiveBounds[0].y);

			ImGuizmo::Manipulate(
				gem::value_ptr(myEditorCameraController->GetCamera()->GetView()),
				gem::value_ptr(myEditorCameraController->GetCamera()->GetProjection()),
				myGizmoOperation, myGizmoMode, gem::value_ptr(averageTransform), nullptr, snap ? snapValues : nullptr);


			isUsing = ImGuizmo::IsUsing();

			if (isUsing)
			{
				averageStartTransform = CalculateAverageTransform();

				if (duplicate && !hasDuplicated)
				{
					std::vector<Wire::EntityId> duplicated;
					for (const auto& ent : SelectionManager::GetSelectedEntities())
					{
						duplicated.emplace_back(Volt::Entity::Duplicate(myEditorScene->GetRegistry(), ent));
					}

					SelectionManager::DeselectAll();

					for (const auto& ent : duplicated)
					{
						SelectionManager::Select(ent);
					}

					hasDuplicated = true;
				}
				else if (averageTransform != averageStartTransform)
				{
					if (SelectionManager::GetSelectedCount() == 1)
					{
						auto& relationshipComp = myEditorScene->GetRegistry().GetComponent<Volt::RelationshipComponent>(SelectionManager::GetSelectedEntities().front());
						auto& transComp = myEditorScene->GetRegistry().GetComponent<Volt::TransformComponent>(SelectionManager::GetSelectedEntities().front());

						if (myMidEvent == false)
						{
							GizmoCommand<gem::vec3>::GizmoData data;
							data.positionAdress = &transComp.position;
							data.rotationAdress = &transComp.rotation;
							data.scaleAdress = &transComp.scale;
							data.previousPositionValue = transComp.position;
							data.previousRotationValue = transComp.rotation;
							data.previousScaleValue = transComp.scale;

							Ref<GizmoCommand<gem::vec3>> command = CreateRef<GizmoCommand<gem::vec3>>(data);
							EditorCommandStack::PushUndo(command);
							myMidEvent = true;
						}

						if (relationshipComp.Parent != 0)
						{
							Volt::Entity parent(relationshipComp.Parent, myEditorScene.get());
							auto pTransform = myEditorScene->GetWorldSpaceTransform(parent);

							averageTransform = gem::inverse(pTransform) * averageTransform;
						}

						gem::vec3 p = 0.f, r = 0.f, s = 1.f;
						Volt::Math::DecomposeTransform(averageTransform, p, r, s);

						gem::vec3 deltaRot = r - transComp.rotation;

						transComp.position = p;
						transComp.rotation += deltaRot;
						transComp.scale = s;
					}
					else
					{
						gem::vec3 newPos, newRot, newScale, oldPos, oldRot, oldScale;

						gem::decompose(averageTransform, newPos, newRot, newScale);
						gem::decompose(averageStartTransform, oldPos, oldRot, oldScale);

						gem::vec3 deltaPos = newPos - oldPos;
						gem::vec3 deltaScale = newScale - oldScale;

						for (const auto& ent : SelectionManager::GetSelectedEntities())
						{
							auto& relationshipComp = myEditorScene->GetRegistry().GetComponent<Volt::RelationshipComponent>(ent);
							auto& transComp = myEditorScene->GetRegistry().GetComponent<Volt::TransformComponent>(ent);

							gem::vec3 p = 0.f, r = 0.f, s = 1.f;
							Volt::Math::DecomposeTransform(averageTransform, p, r, s);

							gem::vec3 deltaRot = r - transComp.rotation;

							transComp.position += deltaPos;
							transComp.rotation += deltaRot;
							transComp.scale += deltaScale;
						}
					}
				}
			}
			else
			{
				hasDuplicated = false;
			}
		}

		myEditorCameraController->SetIsControllable(myIsFocused && !isUsing);
	}

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered() && !ImGuizmo::IsOver() && mySceneState != SceneState::Play)
	{
		gem::vec2 perspectiveSize = myPerspectiveBounds[1] - myPerspectiveBounds[0];

		int32_t mouseX = (int32_t)myViewportMouseCoords.x;
		int32_t mouseY = (int32_t)myViewportMouseCoords.y;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int32_t)perspectiveSize.x && mouseY < (int32_t)perspectiveSize.y)
		{
			uint32_t pixelData = mySceneRenderer->GetSelectionFramebuffer()->ReadPixel<uint32_t>(5, mouseX, mouseY);
			const bool multiSelect = Volt::Input::IsKeyDown(VT_KEY_LEFT_SHIFT);
			const bool deselect = Volt::Input::IsKeyDown(VT_KEY_LEFT_CONTROL);

			if (!multiSelect && !deselect)
			{
				SelectionManager::DeselectAll();
			}

			if (pixelData != Wire::NullID)
			{
				if (deselect)
				{
					SelectionManager::Deselect(pixelData);
				}
				else
				{
					SelectionManager::Select(pixelData);
				}
			}
		}
	}

	ImGui::PopStyleVar(3);

	if (EditorUtils::MeshImportModal("Import Mesh##viewport", myMeshImportData, myMeshToImport))
	{
		Volt::Entity tempEnt{ myEntityToAddMesh, myEditorScene.get() };

		auto& meshComp = tempEnt.AddComponent<Volt::MeshComponent>();
		auto mesh = Volt::AssetManager::GetAsset<Volt::Mesh>(myMeshImportData.destination);
		Volt::AssetManager::Get().SaveAsset(mesh);
		if (mesh)
		{
			meshComp.handle = mesh->handle;
		}
	}

	if (SaveReturnState returnState = EditorUtils::SaveFilePopup("Do you want to save scene?##OpenSceneViewport"); returnState != SaveReturnState::None)
	{
		if (returnState == SaveReturnState::Save)
		{
			Sandbox::Get().SaveScene();
		}

		if (myEditorScene->handle == mySceneToOpen)
		{
			Volt::AssetManager::Get().ReloadAsset(myEditorScene->handle);
		}

		myEditorScene = Volt::AssetManager::GetAsset<Volt::Scene>(mySceneToOpen);
		mySceneRenderer = CreateRef<Volt::SceneRenderer>(myEditorScene);

		Volt::OnSceneLoadedEvent loadEvent{ myEditorScene };
		Volt::Application::Get().OnEvent(loadEvent);

		mySceneToOpen = Volt::Asset::Null();
	}
}

void ViewportPanel::UpdateContent()
{
	if (myMidEvent && Volt::Input::IsMouseButtonReleased(VT_MOUSE_BUTTON_LEFT))
	{
		myMidEvent = false;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 2.f));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0.f, 0.f));
	UI::ScopedColor button(ImGuiCol_Button, { 0.f, 0.f, 0.f, 0.f });
	UI::ScopedColor hovered(ImGuiCol_ButtonHovered, { 0.3f, 0.305f, 0.31f, 0.5f });
	UI::ScopedColor active(ImGuiCol_ButtonActive, { 0.5f, 0.505f, 0.51f, 0.5f });

	ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	const uint32_t rightButtonCount = 9;
	const float buttonSize = 22.f;

	float size = ImGui::GetWindowHeight() - 4.f;
	Ref<Volt::Texture2D> playIcon = EditorIconLibrary::GetIcon(EditorIcon::Play);
	if (mySceneState == SceneState::Play)
	{
		playIcon = EditorIconLibrary::GetIcon(EditorIcon::Stop);
	}

	if (UI::ImageButton("##play", UI::GetTextureID(playIcon), { buttonSize, buttonSize }))
	{
		if (mySceneState == SceneState::Edit)
		{
			Sandbox::Get().OnScenePlay();
			Volt::ViewportResizeEvent resizeEvent{ (uint32_t)myPerspectiveBounds[0].x, (uint32_t)myPerspectiveBounds[0].y, (uint32_t)myViewportSize.x, (uint32_t)myViewportSize.y };
			Volt::Application::Get().OnEvent(resizeEvent);
		}
		else if (mySceneState == SceneState::Play)
		{
			Sandbox::Get().OnSceneStop();
		}

	}

	ImGui::SameLine();

	Ref<Volt::Texture2D> physicsIcon = myAnimatedPhysicsIcon.GetCurrentFrame();
	static Volt::Texture2D* physicsId = physicsIcon.get();

	if (ImGui::ImageButtonAnimated(UI::GetTextureID(physicsId), UI::GetTextureID(physicsIcon), { size, size }, { 0.f, 0.f }, { 1.f, 1.f }, 0))
	{
		if (mySceneState == SceneState::Edit)
		{
			Sandbox::Get().OnSimulationStart();
			myAnimatedPhysicsIcon.Play();
		}
		else if (mySceneState == SceneState::Simulating)
		{
			Sandbox::Get().OnSimulationStop();
			myAnimatedPhysicsIcon.Stop();
		}
	}

	ImGui::SameLine(ImGui::GetContentRegionAvail().x - (rightButtonCount * buttonSize));

	Ref<Volt::Texture2D> localWorldIcon;
	if (myIsWorldSpace)
	{
		localWorldIcon = EditorIconLibrary::GetIcon(EditorIcon::WorldSpace);
	}
	else
	{
		localWorldIcon = EditorIconLibrary::GetIcon(EditorIcon::LocalSpace);
	}

	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.f);
	if (UI::ImageButton("##localWorld", UI::GetTextureID(localWorldIcon), { buttonSize, buttonSize }))
	{
		myIsWorldSpace = !myIsWorldSpace;
	}

	ImGui::SameLine();

	if (UI::ImageButtonState("##snapToGrid", mySnapToGrid, UI::GetTextureID(EditorIconLibrary::GetIcon(EditorIcon::SnapGrid)), { buttonSize, buttonSize }))
	{
		mySnapToGrid = !mySnapToGrid;
	}

	ImGui::SetNextWindowSize({ 100.f, m_snapToGridValues.size() * 20.f });
	if (ImGui::BeginPopupContextItem("##gridSnapValues", ImGuiPopupFlags_MouseButtonRight))
	{
		for (uint32_t i = 0; i < m_snapToGridValues.size(); i++)
		{
			std::string valueStr = Utility::RemoveTrailingZeroes(std::to_string(m_snapToGridValues[i]));
			std::string	id = valueStr + "##gridSnapValue" + std::to_string(i);

			if (ImGui::Selectable(id.c_str()))
			{
				m_gridSnapValue = m_snapToGridValues[i];
			}
		}

		ImGui::EndPopup();
	}
	ImGui::SameLine();

	if (UI::ImageButtonState("##snapRotation", mySnapRotation, UI::GetTextureID(EditorIconLibrary::GetIcon(EditorIcon::SnapRotation)), { buttonSize, buttonSize }))
	{
		mySnapRotation = !mySnapRotation;
	}
	if (ImGui::BeginPopupContextItem("##rotateSnapValues", ImGuiPopupFlags_MouseButtonRight))
	{
		for (uint32_t i = 0; i < m_snapRotationValues.size(); i++)
		{
			std::string valueStr = Utility::RemoveTrailingZeroes(std::to_string(m_snapRotationValues[i]));
			std::string	id = valueStr + "##rotationSnapValue" + std::to_string(i);

			if (ImGui::Selectable(id.c_str()))
			{
				m_rotateSnapValue = m_snapRotationValues[i];
			}
		}

		ImGui::EndPopup();
	}

	ImGui::SameLine();

	if (UI::ImageButtonState("##snapScale", mySnapScale, UI::GetTextureID(EditorIconLibrary::GetIcon(EditorIcon::SnapScale)), { buttonSize, buttonSize }))
	{
		mySnapScale = !mySnapScale;
	}
	if (ImGui::BeginPopupContextItem("##scaleSnapValues", ImGuiPopupFlags_MouseButtonRight))
	{
		for (uint32_t i = 0; i < m_snapScaleValues.size(); i++)
		{
			std::string valueStr = Utility::RemoveTrailingZeroes(std::to_string(m_snapScaleValues[i]));
			std::string	id = valueStr + "##scaleSnapValue" + std::to_string(i);

			if (ImGui::Selectable(id.c_str()))
			{
				m_scaleSnapValue = m_snapScaleValues[i];
			}
		}

		ImGui::EndPopup();
	}

	ImGui::SameLine();

	if (UI::ImageButtonState("##showGizmos", myShowGizmos, UI::GetTextureID(EditorIconLibrary::GetIcon(EditorIcon::ShowGizmos)), { buttonSize, buttonSize }))
	{
		myShowGizmos = !myShowGizmos;
	}

	ImGui::PopStyleVar(3);
	ImGui::End();
}

void ViewportPanel::OnEvent(Volt::Event& e)
{
	Volt::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Volt::MouseMovedEvent>(VT_BIND_EVENT_FN(ViewportPanel::OnMouseMoved));
	dispatcher.Dispatch<Volt::KeyPressedEvent>(VT_BIND_EVENT_FN(ViewportPanel::OnKeyPressedEvent));
	dispatcher.Dispatch<Volt::MouseButtonPressedEvent>(VT_BIND_EVENT_FN(ViewportPanel::OnMousePressed));

	myAnimatedPhysicsIcon.OnEvent(e);
}

bool ViewportPanel::OnMouseMoved(Volt::MouseMovedEvent& e)
{
	Volt::MouseMovedViewportEvent moved{ myViewportMouseCoords.x, myViewportMouseCoords.y };
	Volt::Application::Get().OnEvent(moved);
	return false;
}

bool ViewportPanel::OnMousePressed(Volt::MouseButtonPressedEvent& e)
{
	switch (e.GetMouseButton())
	{
		case VT_MOUSE_BUTTON_RIGHT:
			if (myIsHovered)
			{
				ImGui::SetWindowFocus("Viewport");
			}
			break;
	}

	return false;
}

bool ViewportPanel::OnKeyPressedEvent(Volt::KeyPressedEvent& e)
{
	if (!myIsHovered || Volt::Input::IsMouseButtonPressed(VT_MOUSE_BUTTON_RIGHT))
	{
		return false;
	}

	const bool ctrlPressed = Volt::Input::IsKeyDown(VT_KEY_LEFT_CONTROL);

	switch (e.GetKeyCode())
	{
		case VT_KEY_W:
		{
			myGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
			break;
		}

		case VT_KEY_E:
		{
			myGizmoOperation = ImGuizmo::OPERATION::ROTATE;
			break;
		}

		case VT_KEY_R:
		{
			myGizmoOperation = ImGuizmo::OPERATION::SCALE;
			break;
		}
	}

	return false;
}

gem::mat4 ViewportPanel::CalculateAverageTransform()
{
	gem::mat4 result;

	gem::vec3 avgTranslation;
	gem::vec3 avgRotation;
	gem::vec3 avgScale;

	for (const auto& ent : SelectionManager::GetSelectedEntities())
	{
		gem::vec3 p = 0.f, r = 0.f, s = 1.f;
		gem::mat4 transform = myEditorScene->GetWorldSpaceTransform(Volt::Entity{ ent, myEditorScene.get() });

		gem::decompose(transform, p, r, s);

		avgTranslation += p;
		avgRotation = r;
		avgScale += s;
	}

	avgTranslation /= (float)SelectionManager::GetSelectedCount();
	avgScale /= (float)SelectionManager::GetSelectedCount();

	return gem::translate(gem::mat4(1.f), avgTranslation) * gem::mat4_cast(gem::quat(avgRotation)) * gem::scale(gem::mat4(1.f), avgScale);
}
