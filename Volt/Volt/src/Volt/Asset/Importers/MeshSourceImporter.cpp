#include "vtpch.h"
#include "MeshSourceImporter.h"

#include "MeshTypeImporter.h"
#include "Volt/Log/Log.h"

#include "Volt/Asset/Mesh/Mesh.h"

namespace Volt
{
	bool MeshSourceImporter::Load(const std::filesystem::path& path, Ref<Asset>& asset) const
	{
		asset = CreateRef<Mesh>();

		if (!std::filesystem::exists(path)) [[unlikely]]
		{
			VT_CORE_ERROR("File {0} not found!", path.string().c_str());
			asset->SetFlag(AssetFlag::Missing, true);
			return false;
		}
		auto mesh = MeshTypeImporter::ImportMesh(path);

		if (!mesh) [[unlikely]]
		{
			asset->SetFlag(AssetFlag::Invalid, true);
			return false;
		}

		asset = mesh;
		asset->path = path;
		return true;
	}

	void MeshSourceImporter::Save(const Ref<Asset>& asset) const
	{

	}
}