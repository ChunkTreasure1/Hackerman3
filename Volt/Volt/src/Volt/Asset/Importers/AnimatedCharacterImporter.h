#pragma once

#include "Volt/Asset/Importers/AssetImporter.h"

namespace Volt
{
	class AnimatedCharacterImporter : public AssetImporter
	{
	public:
		bool Load(const std::filesystem::path& path, Ref<Asset>& asset) const override;
		void Save(const Ref<Asset>& asset) const override;
	};
}