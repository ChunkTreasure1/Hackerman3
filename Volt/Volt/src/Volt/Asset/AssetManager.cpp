#include "vtpch.h"
#include "AssetManager.h"

#include "Volt/Asset/Importers/AssetImporter.h"

#include "Volt/Asset/Importers/MeshTypeImporter.h"
#include "Volt/Asset/Importers/TextureImporter.h"
#include "Volt/Asset/Importers/MeshSourceImporter.h"
//#include "Lamp/Asset/Importers/MeshImporter.h"
//#include "Lamp/Asset/Importers/RenderPipelineImporter.h"
//#include "Lamp/Asset/Importers/RenderPassImporter.h"
//#include "Lamp/Asset/Importers/RenderGraphImporter.h"
#include "Volt/Asset/Importers/SkeletonImporter.h"
#include "Volt/Asset/Importers/AnimationImporter.h"
#include "Volt/Asset/Importers/SceneImporter.h"
#include "Volt/Asset/Importers/AnimatedCharacterImporter.h"
#include "Volt/Asset/Importers/PrefabImporter.h"

#include "Volt/Core/Base.h"
#include "Volt/Log/Log.h"

#include "Volt/Utility/StringUtility.h"
#include "Volt/Utility/FileSystem.h"
#include "Volt/Asset/Importers/ParticlePresetImporter.h"

#include <yaml-cpp/yaml.h>

namespace Volt
{
	static const std::filesystem::path s_assetRegistryPath = "Assets/AssetRegistry.vtreg";

	AssetManager::AssetManager()
	{
		VT_CORE_ASSERT(!s_instance, "AssetManager already exists!");
		s_instance = this;

		Initialize();
	}

	AssetManager::~AssetManager()
	{
		Shutdown();
		s_instance = nullptr;
	}

	void AssetManager::Initialize()
	{
		MeshTypeImporter::Initialize();
		TextureImporter::Initialize();

		myAssetImporters.emplace(AssetType::MeshSource, CreateScope<MeshSourceImporter>());
		myAssetImporters.emplace(AssetType::Texture, CreateScope<TextureSourceImporter>());
		myAssetImporters.emplace(AssetType::Shader, CreateScope<ShaderImporter>());
		myAssetImporters.emplace(AssetType::Material, CreateScope<MaterialImporter>());
		myAssetImporters.emplace(AssetType::Mesh, CreateScope<MeshSourceImporter>());
		myAssetImporters.emplace(AssetType::Scene, CreateScope<SceneImporter>());
		myAssetImporters.emplace(AssetType::Skeleton, CreateScope<SkeletonImporter>());
		myAssetImporters.emplace(AssetType::Animation, CreateScope<AnimationImporter>());
		myAssetImporters.emplace(AssetType::AnimatedCharacter, CreateScope<AnimatedCharacterImporter>());
		myAssetImporters.emplace(AssetType::ParticlePreset, CreateScope<ParticlePresetImporter>());
		myAssetImporters.emplace(AssetType::Prefab, CreateScope<PrefabImporter>());
		myAssetImporters.emplace(AssetType::Font, CreateScope<FontImporter>());
		myAssetImporters.emplace(AssetType::PhysicsMaterial, CreateScope<PhysicsMaterialImporter>());
		myAssetImporters.emplace(AssetType::Video, CreateScope<VideoImporter>());

		LoadAssetRegistry();
	}

	void AssetManager::Shutdown()
	{
		SaveAssetRegistry();
		TextureImporter::Shutdown();
		MeshTypeImporter::Shutdown();
	}

	void AssetManager::LoadAsset(const std::filesystem::path& path, Ref<Asset>& asset)
	{
		AssetHandle handle = Asset::Null();
		if (myAssetRegistry.find(path) != myAssetRegistry.end())
		{
			handle = myAssetRegistry.at(path);
		}

		if (handle != Asset::Null() && myAssetCache.find(handle) != myAssetCache.end())
		{
			asset = myAssetCache[handle];
			return;
		}

		const auto type = GetAssetTypeFromPath(path);

		if (myAssetImporters.find(type) == myAssetImporters.end())
		{
			VT_CORE_ERROR("No importer for asset found!");
			return;
		}

#ifdef VT_DEBUG
		VT_CORE_INFO("Loading asset {0}!", path.string().c_str());
#endif

		myAssetImporters[type]->Load(path, asset);
		if (handle != Asset::Null())
		{
			asset->handle = handle;
		}
		else
		{
			myAssetRegistry.emplace(path, asset->handle);
		}

		asset->path = path;
		myAssetCache.emplace(asset->handle, asset);
	}

	void AssetManager::LoadAsset(AssetHandle assetHandle, Ref<Asset>& asset)
	{
		auto it = myAssetCache.find(assetHandle);
		if (it != myAssetCache.end())
		{
			asset = it->second;
			return;
		}

		const auto path = GetPathFromAssetHandle(assetHandle);
		if (!path.empty())
		{
			LoadAsset(path, asset);
		}
	}

	void AssetManager::Unload(AssetHandle assetHandle)
	{
		auto it = myAssetCache.find(assetHandle);
		if (it == myAssetCache.end())
		{
			VT_CORE_WARN("Unable to unload asset with handle {0}, it has not been loaded!", assetHandle);
			return;
		}

		myAssetCache.erase(it);
	}

	void AssetManager::ReloadAsset(const std::filesystem::path& path)
	{
		AssetHandle handle = GetAssetHandleFromPath(path);
		if (handle == Asset::Null())
		{
			VT_CORE_ERROR("Asset with path {0} is not loaded!", path.string());
			return;
		}

		ReloadAsset(handle);
	}

	void AssetManager::ReloadAsset(AssetHandle handle)
	{
		Unload(handle);

		Ref<Asset> asset;
		LoadAsset(handle, asset);
	}

	void AssetManager::SaveAsset(const Ref<Asset> asset)
	{
		if (myAssetImporters.find(asset->GetType()) == myAssetImporters.end())
		{
			VT_CORE_ERROR("No exporter for asset found!");
			return;
		}

		if (!asset->IsValid())
		{
			VT_CORE_ERROR("Unable to save invalid asset!");
			return;
		}

		myAssetImporters[asset->GetType()]->Save(asset);

		if (myAssetRegistry.find(asset->path) == myAssetRegistry.end())
		{
			myAssetRegistry.emplace(asset->path, asset->handle);
		}

		if (myAssetCache.find(asset->handle) == myAssetCache.end())
		{
			myAssetCache.emplace(asset->handle, asset);
		}

		SaveAssetRegistry();
	}

	void AssetManager::MoveAsset(Ref<Asset> asset, const std::filesystem::path& targetDir)
	{
		FileSystem::Move(asset->path, targetDir);

		const std::filesystem::path newPath = targetDir / asset->path.filename();

		myAssetRegistry.erase(asset->path);
		asset->path = newPath;

		myAssetRegistry.emplace(asset->path, asset->handle);
	}

	void AssetManager::MoveAsset(AssetHandle asset, const std::filesystem::path& targetDir)
	{
		const std::filesystem::path oldPath = GetPathFromAssetHandle(asset);
		const std::filesystem::path newPath = targetDir / oldPath.filename();
		FileSystem::Move(oldPath, targetDir);

		myAssetRegistry.erase(oldPath);
		myAssetRegistry.emplace(newPath, asset);
	}

	void AssetManager::RenameAsset(AssetHandle asset, const std::string& newName)
	{
		const std::filesystem::path oldPath = GetPathFromAssetHandle(asset);
		const std::filesystem::path newPath = oldPath.parent_path() / (newName + oldPath.extension().string());
		FileSystem::Rename(GetPathFromAssetHandle(asset), newName);

		myAssetRegistry.erase(oldPath);
		auto it = myAssetCache.find(asset);
		if (it != myAssetCache.end())
		{
			it->second->path = newPath;
		}
		myAssetRegistry.emplace(newPath, asset);
	}

	void AssetManager::RemoveAsset(AssetHandle asset)
	{
		const std::filesystem::path path = GetPathFromAssetHandle(asset);
		myAssetRegistry.erase(path);
		myAssetCache.erase(asset);

		FileSystem::Remove(path);
	}

	void AssetManager::RemoveFromRegistry(AssetHandle asset)
	{
		const std::filesystem::path path = GetPathFromAssetHandle(asset);

		if (!path.empty())
		{
			myAssetRegistry.erase(path);
		}
		else
		{
			VT_CORE_WARN("Asset {0} does not exist in registry!", asset);
		}
	}

	bool AssetManager::IsLoaded(AssetHandle handle) const
	{
		return myAssetCache.find(handle) != myAssetCache.end();
	}

	Ref<Asset> AssetManager::GetAssetRaw(AssetHandle assetHandle)
	{
		auto it = myAssetCache.find(assetHandle);
		if (it != myAssetCache.end())
		{
			return it->second;
		}

		Ref<Asset> asset;
		LoadAsset(assetHandle, asset);

		return asset;
	}

	AssetType AssetManager::GetAssetTypeFromHandle(const AssetHandle& handle) const
	{
		return GetAssetTypeFromExtension(GetPathFromAssetHandle(handle).extension().string());
	}

	AssetType AssetManager::GetAssetTypeFromPath(const std::filesystem::path& path)
	{
		return GetAssetTypeFromExtension(path.extension().string());
	}

	AssetType AssetManager::GetAssetTypeFromExtension(const std::string& extension) const
	{
		std::string ext = Utility::ToLower(extension);
		if (s_assetExtensionsMap.find(ext) == s_assetExtensionsMap.end()) [[unlikely]]
		{
			return AssetType::None;
		}

		return s_assetExtensionsMap.at(ext);
	}

	AssetHandle AssetManager::GetAssetHandleFromPath(const std::filesystem::path& path)
	{
		auto it = myAssetRegistry.find(path);
		if (it == myAssetRegistry.end())
		{
			myAssetRegistry[path] = AssetHandle();
		}
		return myAssetRegistry[path];
	}

	std::filesystem::path AssetManager::GetPathFromAssetHandle(AssetHandle handle) const
	{
		for (const auto& [path, asset] : myAssetRegistry)
		{
			if (asset == handle)
			{
				return path;
			}
		}

		return "";
	}

	std::string AssetManager::GetExtensionFromAssetType(AssetType type) const
	{
		for (const auto& [ext, asset] : s_assetExtensionsMap)
		{
			if (asset == type)
			{
				return ext;
			}
		}
		return "Null";
	}

	bool AssetManager::IsSourceFile(AssetHandle handle) const
	{
		const AssetType type = GetAssetTypeFromHandle(handle);
		switch (type)
		{
			case AssetType::MeshSource:
			case AssetType::ShaderSource:
				return true;
		}
		return false;
	}

	void AssetManager::SaveAssetRegistry()
	{
		YAML::Emitter out;
		out << YAML::BeginMap;

		out << YAML::Key << "Assets" << YAML::BeginSeq;
		for (const auto& [path, handle] : myAssetRegistry)
		{
			if (!IsSourceFile(handle))
			{
				out << YAML::BeginMap;
				out << YAML::Key << "Handle" << YAML::Value << handle;
				out << YAML::Key << "Path" << YAML::Value << FileSystem::GetPathRelativeToBaseFolder(path).string();
				out << YAML::EndMap;
			}
		}
		out << YAML::EndSeq;

		std::ofstream fout(s_assetRegistryPath);
		fout << out.c_str();
		fout.close();
	}

	void AssetManager::LoadAssetRegistry()
	{
		if (!std::filesystem::exists(s_assetRegistryPath))
		{
			return;
		}

		std::ifstream file(s_assetRegistryPath);
		if (!file.is_open()) [[unlikely]]
		{
			VT_CORE_ERROR("Failed to open asset registry file: {0}!", s_assetRegistryPath.string().c_str());
			return;
		}

		std::stringstream strStream;
		strStream << file.rdbuf();
		file.close();

		YAML::Node root = YAML::Load(strStream.str());
		YAML::Node assets = root["Assets"];

		for (const auto entry : assets)
		{
			std::string path = entry["Path"].as<std::string>();
			AssetHandle handle = entry["Handle"].as<uint64_t>();

			myAssetRegistry.emplace(path, handle);
		}
	}
}