#pragma once

#include "Volt/Asset/Importers/AssetImporter.h"

#include "Volt/Utility/YAMLSerializationHelpers.h"
#include "Volt/Utility/SerializationMacros.h"

#include <Wire/Wire.h>
#include <yaml-cpp/yaml.h>

namespace Volt
{
	class PrefabImporter : public AssetImporter
	{
	public:
		bool Load(const std::filesystem::path& path, Ref<Asset>& asset) const override;
		void Save(const Ref<Asset>& asset) const override;

	private:
		template<typename T>
		inline void DeserializeVector(uint8_t* data, uint32_t offset, YAML::Node node, T defaultValue) const
		{
			std::vector<T>& vector = *(std::vector<T>*) & data[offset];
			for (const auto& vecNode : node)
			{
				VT_DESERIALIZE_PROPERTY(value, vector.emplace_back(), vecNode, defaultValue);
			}
		}

		template<typename T>
		inline void SerializeVector(uint8_t* data, uint32_t offset, YAML::Emitter& out) const
		{
			std::vector<T>& items = *(std::vector<T>*) & data[offset];
			for (const auto& item : items)
			{
				out << YAML::BeginMap;
				VT_SERIALIZE_PROPERTY(value, item, out);
				out << YAML::EndMap;
			}
		}
	};
}