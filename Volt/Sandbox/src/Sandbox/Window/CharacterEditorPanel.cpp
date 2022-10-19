#include "sbpch.h"
#include "CharacterEditorPanel.h"

#include "Sandbox/Window/EditorIconLibrary.h"
#include "Sandbox/Camera/EditorCameraController.h"

#include <Volt/Rendering/Renderer.h>
#include <Volt/Rendering/Framebuffer.h>
#include <Volt/Rendering/Shader/ShaderRegistry.h>
#include <Volt/Animation/AnimationManager.h>

#include <Volt/Asset/Animation/AnimatedCharacter.h>
#include <Volt/Asset/Animation/Animation.h>
#include <Volt/Asset/AssetManager.h>

#include <Volt/Utility/UIUtility.h>

CharacterEditorPanel::CharacterEditorPanel()
	: EditorWindow("Character Editor", true)
{
	myWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	myCameraController = CreateRef<EditorCameraController>(60.f, 1.f, 100000.f);
	myGridShader = Volt::ShaderRegistry::Get("Grid");

	// Forward
	{
		Volt::FramebufferSpecification spec{};

		spec.attachments =
		{
			{ Volt::ImageFormat::RGBA, { 0.1f, 0.1f, 0.1f, 1.f }  },
			{ Volt::ImageFormat::R32UI },
			{ Volt::ImageFormat::DEPTH32F }
		};

		spec.width = 1280;
		spec.height = 720;

		myForwardPass.framebuffer = Volt::Framebuffer::Create(spec);
		myForwardPass.debugName = "Forward";
		myForwardPass.overrideShader = Volt::ShaderRegistry::Get("ForwardPBR");
	}
}

void CharacterEditorPanel::UpdateMainContent()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New Character"))
			{
				myNewCharacterData = {};
				UI::OpenModal("New Character##NewCharacterEditor");
			}

			if (ImGui::MenuItem("Open"))
			{
				const std::filesystem::path path = FileSystem::OpenFile("Animated Character (*.vtchr)\0*.vtchr\0");
				if (!path.empty() && FileSystem::Exists(path))
				{
					myCurrentCharacter = Volt::AssetManager::GetAsset<Volt::AnimatedCharacter>(path);
				}
			}

			if (ImGui::MenuItem("Save"))
			{
				if (myCurrentCharacter)
				{
					Volt::AssetManager::Get().SaveAsset(myCurrentCharacter);
					UI::Notify(NotificationType::Success, "Saved character!", std::format("Character {0} successfully saved!", myCurrentCharacter->path.stem().string()));
				}
			}

			if (ImGui::MenuItem("Save As"))
			{
				if (myCurrentCharacter)
				{

				}
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
}

void CharacterEditorPanel::UpdateContent()
{
	UpdateProperties();
	UpdateAnimations();
	UpdateViewport();
	UpdateToolbar();

	EditorUtils::NewCharacterModal("New Character##NewCharacterEditor", myCurrentCharacter, myNewCharacterData);
}

void CharacterEditorPanel::OnEvent(Volt::Event& e)
{
	Volt::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Volt::AppRenderEvent>(VT_BIND_EVENT_FN(CharacterEditorPanel::OnRenderEvent));
	dispatcher.Dispatch<Volt::AppUpdateEvent>(VT_BIND_EVENT_FN(CharacterEditorPanel::OnUpdateEvent));

	myCameraController->OnEvent(e);
}

void CharacterEditorPanel::OpenAsset(Ref<Volt::Asset> asset)
{
	if (myCurrentCharacter)
	{
		Volt::AssetManager::Get().SaveAsset(asset);
	}

	myCurrentCharacter = std::reinterpret_pointer_cast<Volt::AnimatedCharacter>(asset);
}

bool CharacterEditorPanel::OnRenderEvent(Volt::AppRenderEvent& e)
{
	if (myCurrentCharacter && myCurrentCharacter->IsValid())
	{
		if (myIsPlayingAnim)
		{
			Volt::Renderer::Submit(myCurrentCharacter->GetSkin(), gem::mat4(1.f), myCurrentCharacter->SampleAnimation(myCurrentAnimPlaying, myCurrentAnimStartTime));
		}
		else
		{
			Volt::Renderer::Submit(myCurrentCharacter->GetSkin(), gem::mat4(1.f));
		}
	}

	Volt::DirectionalLight dirLight{};
	dirLight.colorIntensity = { 1.f, 1.f, 1.f, 2.f };
	dirLight.direction = { -0.5f, -0.5f, -0.5f, 0.f };
	Volt::Renderer::SubmitLight(dirLight);

	Volt::Renderer::Begin();

	// Forward
	{
		Volt::Renderer::BeginPass(myForwardPass, myCameraController->GetCamera());
		Volt::Renderer::DispatchRenderCommands();
		Volt::Renderer::SubmitSprite(gem::mat4{ 1.f }, { 1.f, 1.f, 1.f, 1.f });
		Volt::Renderer::DispatchSpritesWithShader(myGridShader);
		Volt::Renderer::EndPass();
	}

	Volt::Renderer::End();
	return false;
}

bool CharacterEditorPanel::OnUpdateEvent(Volt::AppUpdateEvent& e)
{
	if (myIsPlayingAnim && myCurrentCharacter && myCurrentCharacter->IsValid())
	{
		if (myIsLooping && myCurrentAnimStartTime + myCurrentCharacter->GetAnimationDuration(myCurrentAnimPlaying) <= Volt::AnimationManager::globalClock )
		{
			myCurrentAnimStartTime = Volt::AnimationManager::globalClock;
		}
	}

	return false;
}

void CharacterEditorPanel::UpdateToolbar()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 2.f));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0.f, 0.f));
	UI::ScopedColor button(ImGuiCol_Button, { 0.f, 0.f, 0.f, 0.f });
	UI::ScopedColor hovered(ImGuiCol_ButtonHovered, { 0.3f, 0.305f, 0.31f, 0.5f });
	UI::ScopedColor active(ImGuiCol_ButtonActive, { 0.5f, 0.505f, 0.51f, 0.5f });

	ImGui::Begin("##toolbarCharEditor", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	if (UI::ImageButton("##Save", UI::GetTextureID(EditorIconLibrary::GetIcon(EditorIcon::Save)), { myButtonSize, myButtonSize }))
	{
		if (myCurrentCharacter)
		{
			Volt::AssetManager::Get().SaveAsset(myCurrentCharacter);
			UI::Notify(NotificationType::Success, "Saved Character!", std::format("Saved character {0} to file!", myCurrentCharacter->path.stem().string()));
		}
	}

	ImGui::SameLine();

	if (UI::ImageButton("##Load", UI::GetTextureID(EditorIconLibrary::GetIcon(EditorIcon::Open)), { myButtonSize, myButtonSize }))
	{
		const std::filesystem::path characterPath = FileSystem::OpenFile("Animated Character (*.vtchr)\0*.vtchr\0");
		if (!characterPath.empty() && FileSystem::Exists(characterPath))
		{
			myCurrentCharacter = Volt::AssetManager::GetAsset<Volt::AnimatedCharacter>(characterPath);
		}
	}


	ImGui::PopStyleVar(2);
	ImGui::End();
}

void CharacterEditorPanel::UpdateViewport()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 0.f, 0.f });
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2{ 0.f, 0.f });
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.f, 0.f });

	ImGui::Begin("Viewport##charEdit");

	myCameraController->SetIsControllable(ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows));

	auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
	auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
	auto viewportOffset = ImGui::GetWindowPos();

	myPerspectiveBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
	myPerspectiveBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

	ImVec2 viewportSize = ImGui::GetContentRegionAvail();
	if (myViewportSize != (*(gem::vec2*)&viewportSize) && viewportSize.x > 0 && viewportSize.y > 0 && !Volt::Input::IsMouseButtonPressed(VT_MOUSE_BUTTON_LEFT))
	{
		myViewportSize = { viewportSize.x, viewportSize.y };
		myForwardPass.framebuffer->Resize((uint32_t)myViewportSize.x, (uint32_t)myViewportSize.y);
		myCameraController->UpdateProjection((uint32_t)myViewportSize.x, (uint32_t)myViewportSize.y);
	}

	ImGui::Image(UI::GetTextureID(myForwardPass.framebuffer->GetColorAttachment(0)), viewportSize);
	ImGui::End();
	ImGui::PopStyleVar(3);
}

void CharacterEditorPanel::UpdateProperties()
{
	ImGui::Begin("Properties##charEdit");
	if (UI::BeginProperties("CharProperties"))
	{
		UI::Property("Is Looping", myIsLooping);

		UI::EndProperties();
	}
	ImGui::End();
}

void CharacterEditorPanel::UpdateAnimations()
{
	const float buttonSize = 22.f;

	ImGui::Begin("Animations##charEdit");

	if (!myCurrentCharacter)
	{
		ImGui::End();
		return;
	}

	ImGui::BeginChild("AnimBar", { ImGui::GetContentRegionAvail().x, myButtonSize + 5.f }, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	{
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2{ 0.f, 0.f });
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.f, 0.f });

		// Adding
		{
			Volt::AssetHandle addHandle = Volt::Asset::Null();
			if (UI::ImageButton("##Add", UI::GetTextureID(EditorIconLibrary::GetIcon(EditorIcon::Add)), { buttonSize, buttonSize }))
			{
				ImGui::OpenPopup("animAddPopup");
			}

			if (UI::AssetBrowserPopup("animAddPopup", Volt::AssetType::Animation, addHandle))
			{
				if (addHandle != Volt::Asset::Null())
				{
					myCurrentCharacter->SetAnimation((uint32_t)myCurrentCharacter->GetAnimationCount(), Volt::AssetManager::GetAsset<Volt::Animation>(addHandle));
				}
			}
		}

		ImGui::SameLine();

		// Play
		{
			Ref<Volt::Texture2D> icon = myIsPlayingAnim ? EditorIconLibrary::GetIcon(EditorIcon::Stop) : EditorIconLibrary::GetIcon(EditorIcon::Play);
			if (UI::ImageButton("##Play", UI::GetTextureID(icon), { buttonSize, buttonSize }))
			{
				myIsPlayingAnim = !myIsPlayingAnim;
				if (myIsPlayingAnim)
				{
					myCurrentAnimStartTime = Volt::AnimationManager::globalClock;
				}
			}
		}

		ImGui::PopStyleVar(2);
	}
	ImGui::EndChild();

	ImGui::Separator();

	ImGui::BeginChild("Content", ImGui::GetContentRegionAvail());
	{
		if (ImGui::BeginTable("AnimTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_Resizable))
		{
			ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthStretch, 0.3f);
			ImGui::TableSetupColumn("Animation", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableHeadersRow();

			for (const auto& [index, anim] : myCurrentCharacter->GetAnimations())
			{
				ImGui::TableNextColumn();
				ImGui::Text("%d", index);

				ImGui::TableNextColumn();

				std::string animName;
				if (anim && anim->IsValid())
				{
					animName = anim->path.stem().string();
				}
				else
				{
					"Null";
				}

				std::string popupName = "animPopup" + std::to_string(index);

				ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
				UI::InputText("", animName, ImGuiInputTextFlags_ReadOnly);
				if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
				{
					ImGui::OpenPopup(popupName.c_str());
				}
				ImGui::PopItemWidth();

				std::string rightClickId = "animRightClick" + std::to_string(index);
				if (ImGui::BeginPopupContextItem(rightClickId.c_str(), ImGuiPopupFlags_MouseButtonRight))
				{
					if (ImGui::MenuItem("Play") && anim && anim->IsValid())
					{
						myIsPlayingAnim = true;
						myCurrentAnimPlaying = index;
						myCurrentAnimStartTime = Volt::AnimationManager::globalClock;
					}

					ImGui::EndPopup();
				}

				Volt::AssetHandle animHandle = Volt::Asset::Null();
				if (UI::AssetBrowserPopup(popupName, Volt::AssetType::Animation, animHandle))
				{
					if (animHandle != Volt::Asset::Null())
					{
						myCurrentCharacter->SetAnimation(index, Volt::AssetManager::GetAsset<Volt::Animation>(animHandle));
					}
				}
			}

			ImGui::EndTable();
		}
	}
	ImGui::EndChild();

	ImGui::End();
}