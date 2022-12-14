#include "vtpch.h"
#include "ShaderRegistry.h"

#include "Volt/Asset/AssetManager.h"
#include "Volt/Log/Log.h"

#include "Volt/Rendering/Shader/Shader.h"

#include "Volt/Utility/StringUtility.h"
#include "Volt/Utility/FileSystem.h"

namespace Volt
{
	void ShaderRegistry::Initialize()
	{
		LoadAllShaders();
	}

	void ShaderRegistry::Shutdown()
	{
		myRegistry.clear();
	}

	Ref<Shader> ShaderRegistry::Get(const std::string& name)
	{
		std::string lowName = Utility::ToLower(name);
		auto it = myRegistry.find(lowName);
		if (it == myRegistry.end())
		{
			VT_CORE_ERROR("Unable to find shader {0}!", name.c_str());
			return nullptr;
		}

		return it->second;
	}

	void ShaderRegistry::Register(const std::string& name, Ref<Shader> shader)
	{
		auto it = myRegistry.find(name);
		if (it != myRegistry.end())
		{
			VT_CORE_ERROR("Shader with that name has already been registered!");
			return;
		}

		std::string lowName = Utility::ToLower(name);
		myRegistry[lowName] = shader;
	}

	const std::map<std::string, Ref<Shader>>& ShaderRegistry::GetAllShaders()
	{
		return myRegistry;
	}

	void ShaderRegistry::LoadAllShaders()
	{
		auto shaderSearchFolder = FileSystem::GetShadersPath();
		for (const auto& path : std::filesystem::recursive_directory_iterator(shaderSearchFolder))
		{
			AssetType type = AssetManager::Get().GetAssetTypeFromPath(path.path());
			if (type == AssetType::Shader)
			{
				Ref<Shader> shader = AssetManager::GetAsset<Shader>(path.path());
				Register(shader->GetName(), shader);
			}
		}
	}
}