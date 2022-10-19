#pragma once

#include "Volt/Asset/Asset.h"

#include "Volt/Log/Log.h"
#include "Volt/Core/Base.h"
#include "Volt/Core/Profiling.h"

#include <map>
#include <filesystem>
#include <unordered_map>

namespace Volt
{
	class AssetImporter;
	class AssetManager
	{
	public:
		AssetManager();
		~AssetManager();

		void Initialize();
		void Shutdown();

		void LoadAsset(const std::filesystem::path& path, Ref<Asset>& asset);
		void LoadAsset(AssetHandle assetHandle, Ref<Asset>& asset);
		void Unload(AssetHandle assetHandle);
		void SaveAsset(const Ref<Asset> asset);
		
		void MoveAsset(Ref<Asset> asset, const std::filesystem::path& targetDir);
		void MoveAsset(AssetHandle asset, const std::filesystem::path& targetDir);
		void RenameAsset(AssetHandle asset, const std::string& newName);
		void RemoveAsset(AssetHandle asset);

		void RemoveFromRegistry(AssetHandle asset);

		void ReloadAsset(AssetHandle handle);
		void ReloadAsset(const std::filesystem::path& path);

		bool IsLoaded(AssetHandle handle) const;

		inline const std::map<std::filesystem::path, AssetHandle>& GetAssetRegistry() const { return myAssetRegistry; }

		Ref<Asset> GetAssetRaw(AssetHandle assetHandle);

		AssetType GetAssetTypeFromHandle(const AssetHandle& handle) const;
		AssetType GetAssetTypeFromPath(const std::filesystem::path& path);
		AssetType GetAssetTypeFromExtension(const std::string& extension) const;
		AssetHandle GetAssetHandleFromPath(const std::filesystem::path& path);
		std::filesystem::path GetPathFromAssetHandle(AssetHandle handle) const;
		std::string GetExtensionFromAssetType(AssetType type) const;

		inline static AssetManager& Get() { return *s_instance; }

		template<typename T>
		static Ref<T> GetAsset(AssetHandle assetHandle);

		template<typename T>
		static AssetHandle GetHandle(const std::filesystem::path& path);

		template<typename T>
		static Ref<T> GetAsset(const std::filesystem::path& path);

		template<typename T, typename... Args>
		static Ref<T> CreateAsset(const std::filesystem::path& targetDir, const std::string& filename, Args&&... args);

	private:
		inline static AssetManager* s_instance = nullptr;

		void SaveAssetRegistry();
		void LoadAssetRegistry();

		bool IsSourceFile(AssetHandle handle) const;

		std::unordered_map<AssetType, Scope<AssetImporter>> myAssetImporters;
		std::map<std::filesystem::path, AssetHandle> myAssetRegistry;
		std::unordered_map<AssetHandle, Ref<Asset>> myAssetCache;
	};

	template<typename T>
	inline Ref<T> AssetManager::GetAsset(AssetHandle assetHandle)
	{
		VT_PROFILE_FUNCTION();

		if (assetHandle == Asset::Null())
		{
			return nullptr;
		}

		Ref<Asset> asset;
		Get().LoadAsset(assetHandle, asset);

		return std::reinterpret_pointer_cast<T>(asset);
	}

	template<typename T>
	inline AssetHandle AssetManager::GetHandle(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			VT_CORE_ERROR("Unable to load asset {0}! It does not exist!", path.string().c_str());
			return Asset::Null();
		}

		auto it = Get().myAssetRegistry.find(path);
		if (it != Get().myAssetRegistry.end())
		{
			return it->second;
		}

		Ref<T> asset = GetAsset<T>(path);
		if (asset)
		{
			return asset->handle;
		}

		return Asset::Null();
	}

	template<typename T>
	inline Ref<T> AssetManager::GetAsset(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			VT_CORE_ERROR("Unable to load asset {0}! It does not exist!", path.string().c_str());
			return nullptr;
		}

		Ref<Asset> asset;
		Get().LoadAsset(path, asset);

		return std::reinterpret_pointer_cast<T>(asset);
	}

	template<typename T, typename ...Args>
	inline Ref<T> AssetManager::CreateAsset(const std::filesystem::path& targetDir, const std::string& filename, Args && ...args)
	{
		Ref<T> asset = CreateRef<T>(std::forward<Args>(args)...);
		asset->path = targetDir / filename;

		AssetManager::Get().myAssetRegistry.emplace(asset->path, asset->handle);
		AssetManager::Get().myAssetCache.emplace(asset->handle, asset);

		Get().SaveAssetRegistry();

		return asset;
	}
} 