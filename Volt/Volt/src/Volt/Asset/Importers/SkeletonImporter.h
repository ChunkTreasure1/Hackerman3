#pragma once

#include "Volt/Asset/Importers/AssetImporter.h"

namespace Volt
{
	class SkeletonImporter : public AssetImporter
	{
	public:
		bool Load(const std::filesystem::path& path, Ref<Asset>& asset) const override;
		void Save(const Ref<Asset>& asset) const override;

	private:
		struct SkeletonHeader
		{
			uint32_t nameLength = 0;
			uint32_t jointCount = 0;
			uint32_t bindPoseCount = 0;
		};
	};
}