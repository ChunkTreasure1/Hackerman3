#include "sbpch.h"
#include "EditorUtilities.h"

#include <Volt/Asset/Animation/Animation.h>
#include <Volt/Asset/Animation/Skeleton.h>
#include <Volt/Asset/Animation/AnimatedCharacter.h>

#include <Volt/Asset/Mesh/MeshCompiler.h>
#include <Volt/Asset/Importers/MeshTypeImporter.h>

#include <Volt/Utility/UIUtility.h>

bool EditorUtils::MeshImportModal(const std::string& aId, MeshImportData& aImportData, const std::filesystem::path& aMeshToImport)
{
	bool imported = false;

	UI::ScopedStyleFloat rounding{ ImGuiStyleVar_FrameRounding, 2.f };

	if (UI::BeginModal(aId, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_AlwaysAutoResize))
	{
		const std::string srcPath = aMeshToImport.string();

		ImGui::TextUnformatted("Settings");

		UI::PushId();
		if (UI::BeginProperties("Properties"))
		{
			UI::Property("Source", srcPath, true);
			UI::Property("Destination", aImportData.destination);

			UI::EndProperties();
		}
		UI::PopId();

		ImGui::Separator();
		ImGui::TextUnformatted("Mesh");

		UI::PushId();
		if (UI::BeginProperties("Settings"))
		{
			UI::Property("Import Mesh", aImportData.importMesh);
			if (aImportData.importMesh)
			{
				UI::Property("Create Materials", aImportData.createMaterials);
			}

			if (!aImportData.createMaterials && aImportData.importMesh)
			{
				UI::Property("Material", aImportData.externalMaterial, Volt::AssetType::Material);
			}

			UI::EndProperties();
		}

		ImGui::Separator();
		ImGui::TextUnformatted("Animation");

		if (UI::BeginProperties("Settings"))
		{
			UI::Property("Import Skeleton", aImportData.importSkeleton);
			UI::Property("Import Animation", aImportData.importAnimation);

			UI::EndProperties();
		}
		UI::PopId();

		if (ImGui::Button("Import"))
		{
			const Volt::AssetHandle material = aImportData.createMaterials ? Volt::Asset::Null() : aImportData.externalMaterial;
			bool succeded = true;

			if (aImportData.importMesh)
			{
				succeded = succeded && Volt::MeshCompiler::TryCompile(Volt::AssetManager::GetAsset<Volt::Mesh>(aMeshToImport), aImportData.destination, material);
				if (!succeded)
				{
					UI::Notify(NotificationType::Error, "Failed to compile mesh!", std::format("Failed to compile mesh to location {}!", aImportData.destination.string()));
				}
			}

			if (aImportData.importSkeleton)
			{
				Ref<Volt::Skeleton> skeleton = Volt::MeshTypeImporter::ImportSkeleton(aMeshToImport);
				if (!skeleton)
				{
					UI::Notify(NotificationType::Error, "Failed to import skeleton!", std::format("Failed to import skeleton from {}!", aMeshToImport.string()));
				}
				else
				{
					skeleton->path = aImportData.destination.parent_path() / (aImportData.destination.stem().string() + ".vtsk");
					Volt::AssetManager::Get().SaveAsset(skeleton);
				}
			}

			if (aImportData.importAnimation)
			{
				Ref<Volt::Animation> animation = Volt::MeshTypeImporter::ImportAnimation(aMeshToImport);
				if (!animation)
				{
					UI::Notify(NotificationType::Error, "Failed to import animation!", std::format("Failed to import animaition from {}!", aMeshToImport.string()));
				}
				else
				{
					animation->path = aImportData.destination.parent_path() / (aImportData.destination.stem().string() + ".vtanim");
					Volt::AssetManager::Get().SaveAsset(animation);
				}
			}

			if (succeded)
			{
				UI::Notify(NotificationType::Success, "Mesh compilation succeded!", std::format("Successfully compiled mesh to {}!", aImportData.destination.string()));
				imported = true;
			}
			else
			{
			}

			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}

		UI::EndModal();
	}
	return imported;
}

bool EditorUtils::NewCharacterModal(const std::string& aId, Ref<Volt::AnimatedCharacter>& outCharacter, NewCharacterData& aCharacterData)
{
	bool created = false;

	UI::ScopedStyleFloat rounding{ ImGuiStyleVar_FrameRounding, 2.f };
	if (UI::BeginModal(aId, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
	{
		UI::ShiftCursor(300.f, 0.f);
		UI::ShiftCursor(-300.f, 0.f);

		if (UI::BeginProperties("NewCharacter"))
		{
			UI::Property("Name", aCharacterData.name);
			UI::Property("Skeleton", aCharacterData.skeletonHandle, Volt::AssetType::Skeleton);
			UI::Property("Skin", aCharacterData.skinHandle, Volt::AssetType::Mesh);
			UI::PropertyDirectory("Destination", aCharacterData.destination);

			UI::EndProperties();
		}

		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Create"))
		{
			created = true;
			outCharacter = Volt::AssetManager::CreateAsset<Volt::AnimatedCharacter>(FileSystem::GetPathRelativeToBaseFolder(aCharacterData.destination), aCharacterData.name + ".vtchr");

			if (aCharacterData.skeletonHandle != Volt::Asset::Null())
			{
				outCharacter->SetSkeleton(Volt::AssetManager::GetAsset<Volt::Skeleton>(aCharacterData.skeletonHandle));
			}

			if (aCharacterData.skinHandle != Volt::Asset::Null())
			{
				outCharacter->SetSkin(Volt::AssetManager::GetAsset<Volt::Mesh>(aCharacterData.skinHandle));
			}

			Volt::AssetManager::Get().SaveAsset(outCharacter);
			ImGui::CloseCurrentPopup();
		}

		UI::EndModal();
	}

	return created;
}

SaveReturnState EditorUtils::SaveFilePopup(const std::string& aId)
{
	SaveReturnState returnState = SaveReturnState::None;
	UI::ScopedStyleFloat rounding{ ImGuiStyleVar_FrameRounding, 2.f };

	if (UI::BeginModal(aId, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
	{
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Discard"))
		{
			returnState = SaveReturnState::Discard;
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		
		if (ImGui::Button("Save"))
		{
			returnState = SaveReturnState::Save;
			ImGui::CloseCurrentPopup();
		}

		UI::EndModal();
	}

	return returnState;
}
