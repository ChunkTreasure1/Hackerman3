#pragma once

#include "AssetImporter.h"

namespace Volt
{
	class MeshSourceImporter : public AssetImporter
	{
	public:
		bool Load(const std::filesystem::path& path, Ref<Asset>& asset) const override;
		void Save(const Ref<Asset>& asset) const override;
	};
}