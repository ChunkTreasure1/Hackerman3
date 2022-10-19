#pragma once

#include "Sandbox/Window/AssetCommon.h"

#include <Volt/Asset/Asset.h>
#include <Volt/Utility/UIUtility.h>

#include <filesystem>
#include <format>

struct MeshImportData
{
	std::filesystem::path destination;
	Volt::AssetHandle externalMaterial;
	bool createMaterials = true;
	bool importMesh = true;
	bool importSkeleton = false;
	bool importAnimation = false;
};

struct NewCharacterData
{
	std::string name = "None";
	Volt::AssetHandle skeletonHandle = Volt::Asset::Null();
	Volt::AssetHandle skinHandle = Volt::Asset::Null();
	std::filesystem::path destination = "Assets/Animations/";
};

enum class SaveReturnState
{
	None,
	Save,
	Discard
};

class EditorUtils
{
public:
	static bool MeshImportModal(const std::string& aId, MeshImportData& aImportData, const std::filesystem::path& aMeshToImport);
	static bool NewCharacterModal(const std::string& aId, Ref<Volt::AnimatedCharacter>& outCharacter, NewCharacterData& aCharacterData);

	static SaveReturnState SaveFilePopup(const std::string& aId);

private:
	EditorUtils() = delete;
};