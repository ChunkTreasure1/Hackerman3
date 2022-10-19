#include "vtpch.h"
#include "SceneImporter.h"

#include "Volt/Scene/Scene.h"

#include "Volt/Utility/SerializationMacros.h"
#include "Volt/Utility/YAMLSerializationHelpers.h"

#include "Volt/Components/Components.h"

#include "Volt/Log/Log.h"
#include "Volt/Utility/FileSystem.h"

#include <Wire/Serialization.h>

namespace Volt
{
	bool SceneImporter::Load(const std::filesystem::path& path, Ref<Asset>& asset) const
	{
		asset = CreateRef<Scene>();
		Ref<Scene> scene = reinterpret_pointer_cast<Scene>(asset);

		if (!std::filesystem::exists(path)) [[unlikely]]
		{
			VT_CORE_ERROR("File {0} not found!", path.string().c_str());
			asset->SetFlag(AssetFlag::Missing, true);
			return false;
		}

		std::ifstream file(path);
		if (!file.is_open()) [[unlikely]]
		{
			VT_CORE_ERROR("Failed to open file {0}!", path.string().c_str());
			asset->SetFlag(AssetFlag::Invalid, true);
			return false;
		}

		const std::filesystem::path& scenePath = path;
		std::filesystem::path folderPath = scenePath.parent_path();
		std::filesystem::path entitiesFolderPath = folderPath / "Entities";

		std::stringstream sstream;
		sstream << file.rdbuf();
		file.close();

		YAML::Node root;

		try
		{
			root = YAML::Load(sstream.str());
		}
		catch (std::exception& e)
		{
			VT_CORE_ERROR("{0} contains invalid YAML! Please correct it!", path);
			asset->SetFlag(AssetFlag::Invalid, true);
			return false;
		}

		YAML::Node sceneNode = root["Scene"];

		VT_DESERIALIZE_PROPERTY(name, scene->myName, sceneNode, std::string("New Scene"));

		if (std::filesystem::exists(entitiesFolderPath))
		{
			for (const auto& it : std::filesystem::directory_iterator(entitiesFolderPath))
			{
				DeserializeEntity(it.path(), scene->myRegistry);
			}
		}

		return true;
	}

	void SceneImporter::Save(const Ref<Asset>& asset) const
	{
		const Ref<Scene> scene = std::reinterpret_pointer_cast<Scene>(asset);

		std::filesystem::path folderPath = asset->path;
		if (!std::filesystem::is_directory(folderPath))
		{
			folderPath = folderPath.parent_path();
		}

		std::filesystem::path scenePath = folderPath / (asset->path.stem().string() + ".vtscene");
		std::filesystem::path entitiesFolderPath = folderPath / "Entities";

		if (!std::filesystem::exists(folderPath))
		{
			std::filesystem::create_directories(folderPath);
		}

		if (!std::filesystem::exists(entitiesFolderPath))
		{
			std::filesystem::create_directories(entitiesFolderPath);
		}

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value;
		{
			out << YAML::BeginMap;
			VT_SERIALIZE_PROPERTY(name, scene->GetName(), out);
			out << YAML::EndMap;
		}
		out << YAML::EndMap;

		std::ofstream file;
		file.open(scenePath);
		file << out.c_str();
		file.close();

		/////Entities//////
		for (const auto entity : scene->GetRegistry().GetAllEntities())
		{
			SerializeEntity(entity, scene->GetRegistry(), entitiesFolderPath);
		}

		for (const auto& file : std::filesystem::directory_iterator(entitiesFolderPath))
		{
			if (!file.is_directory())
			{
				const auto stem = file.path().stem().string();
				const size_t underscoreOffset = stem.find('_');

				if (underscoreOffset != std::string::npos)
				{
					const std::string entIdString = stem.substr(underscoreOffset + 1);
					int32_t id = std::stoi(entIdString); // Will break if id > 1.2 billion
					if (!scene->GetRegistry().Exists((uint32_t)id))
					{
						FileSystem::Remove(file.path());
					}
				}
			}

		}

		asset->path = scenePath;
	}

	void SceneImporter::SerializeEntity(Wire::EntityId id, const Wire::Registry& registry, const std::filesystem::path& targetDir) const
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value;
		{
			out << YAML::BeginMap;
			VT_SERIALIZE_PROPERTY(id, id, out);

			out << YAML::Key << "components" << YAML::BeginSeq;
			for (const auto& [guid, pool] : registry.GetPools())
			{
				if (!pool->HasComponent(id))
				{
					continue;
				}

				auto* componentData = (uint8_t*)pool->GetComponent(id);
				const auto& compInfo = Wire::ComponentRegistry::GetRegistryDataFromGUID(guid);

				out << YAML::BeginMap;
				out << YAML::Key << "guid" << YAML::Value << guid;
				out << YAML::Key << "properties" << YAML::BeginSeq;
				for (const auto& prop : compInfo.properties)
				{
					if (prop.serializable)
					{
						out << YAML::BeginMap;
						VT_SERIALIZE_PROPERTY(type, prop.type, out);
						VT_SERIALIZE_PROPERTY(vectorType, prop.vectorType, out);
						VT_SERIALIZE_PROPERTY(name, prop.name, out);
						switch (prop.type)
						{
							case Wire::ComponentRegistry::PropertyType::Bool: VT_SERIALIZE_PROPERTY(data, *(bool*)&componentData[prop.offset], out); break;
							case Wire::ComponentRegistry::PropertyType::Int: VT_SERIALIZE_PROPERTY(data, *(int32_t*)&componentData[prop.offset], out); break;
							case Wire::ComponentRegistry::PropertyType::UInt: VT_SERIALIZE_PROPERTY(data, *(uint32_t*)&componentData[prop.offset], out); break;
							case Wire::ComponentRegistry::PropertyType::Short: VT_SERIALIZE_PROPERTY(data, *(int16_t*)&componentData[prop.offset], out); break;
							case Wire::ComponentRegistry::PropertyType::UShort: VT_SERIALIZE_PROPERTY(data, *(uint32_t*)&componentData[prop.offset], out); break;
							case Wire::ComponentRegistry::PropertyType::Char: VT_SERIALIZE_PROPERTY(data, *(int8_t*)&componentData[prop.offset], out); break;
							case Wire::ComponentRegistry::PropertyType::UChar: VT_SERIALIZE_PROPERTY(data, *(uint8_t*)&componentData[prop.offset], out); break;
							case Wire::ComponentRegistry::PropertyType::Float: VT_SERIALIZE_PROPERTY(data, *(float*)&componentData[prop.offset], out); break;
							case Wire::ComponentRegistry::PropertyType::Double: VT_SERIALIZE_PROPERTY(data, *(double*)&componentData[prop.offset], out); break;
							case Wire::ComponentRegistry::PropertyType::Vector2: VT_SERIALIZE_PROPERTY(data, *(gem::vec2*)&componentData[prop.offset], out); break;
							case Wire::ComponentRegistry::PropertyType::Vector3: VT_SERIALIZE_PROPERTY(data, *(gem::vec3*)&componentData[prop.offset], out); break;
							case Wire::ComponentRegistry::PropertyType::Vector4: VT_SERIALIZE_PROPERTY(data, *(gem::vec4*)&componentData[prop.offset], out); break;
							case Wire::ComponentRegistry::PropertyType::String:
							{
								std::string str = *(std::string*)&componentData[prop.offset];
								VT_SERIALIZE_PROPERTY(data, str, out);
								break;
							}
							case Wire::ComponentRegistry::PropertyType::Int64: VT_SERIALIZE_PROPERTY(data, *(int64_t*)&componentData[prop.offset], out); break;
							case Wire::ComponentRegistry::PropertyType::UInt64: VT_SERIALIZE_PROPERTY(data, *(uint64_t*)&componentData[prop.offset], out); break;
							case Wire::ComponentRegistry::PropertyType::AssetHandle: VT_SERIALIZE_PROPERTY(data, *(AssetHandle*)&componentData[prop.offset], out); break;
							case Wire::ComponentRegistry::PropertyType::Color3: VT_SERIALIZE_PROPERTY(data, *(gem::vec3*)&componentData[prop.offset], out); break;
							case Wire::ComponentRegistry::PropertyType::Color4: VT_SERIALIZE_PROPERTY(data, *(gem::vec4*)&componentData[prop.offset], out); break;
							case Wire::ComponentRegistry::PropertyType::Folder: VT_SERIALIZE_PROPERTY(data, *(std::filesystem::path*)&componentData[prop.offset], out); break;
							case Wire::ComponentRegistry::PropertyType::Path: VT_SERIALIZE_PROPERTY(data, *(std::filesystem::path*)&componentData[prop.offset], out); break;
							case Wire::ComponentRegistry::PropertyType::GUID: VT_SERIALIZE_PROPERTY(data, *(WireGUID*)&componentData[prop.offset], out); break;
							case Wire::ComponentRegistry::PropertyType::EntityId: VT_SERIALIZE_PROPERTY(data, *(uint32_t*)&componentData[prop.offset], out); break;
							case Wire::ComponentRegistry::PropertyType::Enum: VT_SERIALIZE_PROPERTY(data, *(uint32_t*)&componentData[prop.offset], out); break;

							case Wire::ComponentRegistry::PropertyType::Vector:
							{
								out << YAML::Key << "data" << YAML::BeginSeq;
								switch (prop.vectorType)
								{
									case Wire::ComponentRegistry::PropertyType::Bool: SerializeVector<bool>(componentData, prop.offset, out); break;
									case Wire::ComponentRegistry::PropertyType::Int: SerializeVector<int32_t>(componentData, prop.offset, out); break;
									case Wire::ComponentRegistry::PropertyType::UInt: SerializeVector<uint32_t>(componentData, prop.offset, out); break;
									case Wire::ComponentRegistry::PropertyType::Short: SerializeVector<int16_t>(componentData, prop.offset, out); break;
									case Wire::ComponentRegistry::PropertyType::UShort: SerializeVector<uint16_t>(componentData, prop.offset, out); break;
									case Wire::ComponentRegistry::PropertyType::Char: SerializeVector<int8_t>(componentData, prop.offset, out); break;
									case Wire::ComponentRegistry::PropertyType::UChar: SerializeVector<uint8_t>(componentData, prop.offset, out); break;
									case Wire::ComponentRegistry::PropertyType::Float: SerializeVector<float>(componentData, prop.offset, out); break;
									case Wire::ComponentRegistry::PropertyType::Double: SerializeVector<double>(componentData, prop.offset, out); break;
									case Wire::ComponentRegistry::PropertyType::Vector2: SerializeVector<gem::vec2>(componentData, prop.offset, out); break;
									case Wire::ComponentRegistry::PropertyType::Vector3: SerializeVector<gem::vec3>(componentData, prop.offset, out); break;
									case Wire::ComponentRegistry::PropertyType::Vector4: SerializeVector<gem::vec4>(componentData, prop.offset, out); break;
									case Wire::ComponentRegistry::PropertyType::String: SerializeVector<std::string>(componentData, prop.offset, out); break;
									case Wire::ComponentRegistry::PropertyType::Int64: SerializeVector<int64_t>(componentData, prop.offset, out); break;
									case Wire::ComponentRegistry::PropertyType::UInt64: SerializeVector<uint64_t>(componentData, prop.offset, out); break;
									case Wire::ComponentRegistry::PropertyType::AssetHandle: SerializeVector<AssetHandle>(componentData, prop.offset, out); break;
									case Wire::ComponentRegistry::PropertyType::Color3: SerializeVector<gem::vec3>(componentData, prop.offset, out); break;
									case Wire::ComponentRegistry::PropertyType::Color4: SerializeVector<gem::vec4>(componentData, prop.offset, out); break;
									case Wire::ComponentRegistry::PropertyType::Folder: SerializeVector<std::filesystem::path>(componentData, prop.offset, out); break;
									case Wire::ComponentRegistry::PropertyType::Path: SerializeVector<std::filesystem::path>(componentData, prop.offset, out); break;
									case Wire::ComponentRegistry::PropertyType::GUID: SerializeVector<WireGUID>(componentData, prop.offset, out); break;
									case Wire::ComponentRegistry::PropertyType::EntityId: SerializeVector<Wire::EntityId>(componentData, prop.offset, out); break;
									case Wire::ComponentRegistry::PropertyType::Enum: SerializeVector<uint32_t>(componentData, prop.offset, out); break;
								}

								out << YAML::EndSeq;
								break;
							}
						}
						out << YAML::EndMap;
					}
				}
				out << YAML::EndSeq;
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;

			out << YAML::EndMap;
		}
		out << YAML::EndMap;

		std::ofstream output(targetDir / ("ent_" + std::to_string(id) + ".ent"), std::ios::out);
		output << out.c_str();
		output.close();
	}

	void SceneImporter::DeserializeEntity(const std::filesystem::path& path, Wire::Registry& registry) const
	{
		std::ifstream file(path);
		std::stringstream sstream;
		sstream << file.rdbuf();
		file.close();

		YAML::Node root;

		try
		{
			root = YAML::Load(sstream.str());
		}
		catch (std::exception& e)
		{
			VT_CORE_ERROR("{0} contains invalid YAML! Please correct it! Error: {1}", path, e.what());
			return;
		}

		YAML::Node entityNode = root["Entity"];

		Wire::EntityId entityId = Wire::NullID;
		VT_DESERIALIZE_PROPERTY(id, entityId, entityNode, (Wire::EntityId)Wire::NullID);

		if (entityId == Wire::NullID)
		{
			return;
		}

		registry.AddEntity(entityId);

		YAML::Node componentsNode = entityNode["components"];
		if (componentsNode)
		{
			for (auto compNode : componentsNode)
			{
				WireGUID componentGUID;
				VT_DESERIALIZE_PROPERTY(guid, componentGUID, compNode, WireGUID::Null());
				if (componentGUID == WireGUID::Null())
				{
					continue;
				}

				auto* componentData = (uint8_t*)registry.AddComponent(componentGUID, entityId);

				YAML::Node propertiesNode = compNode["properties"];
				if (propertiesNode)
				{
					auto regInfo = Wire::ComponentRegistry::GetRegistryDataFromGUID(componentGUID);

					for (auto propNode : propertiesNode)
					{
						Wire::ComponentRegistry::PropertyType type;
						Wire::ComponentRegistry::PropertyType vectorType;
						std::string name;

						VT_DESERIALIZE_PROPERTY(type, type, propNode, Wire::ComponentRegistry::PropertyType::Unknown);
						VT_DESERIALIZE_PROPERTY(vectorType, vectorType, propNode, Wire::ComponentRegistry::PropertyType::Unknown);
						VT_DESERIALIZE_PROPERTY(name, name, propNode, std::string("Null"));

						if (type == Wire::ComponentRegistry::PropertyType::Unknown)
						{
							continue;
						}

						// Try to find property
						auto it = std::find_if(regInfo.properties.begin(), regInfo.properties.end(), [name, type](const auto& prop)
							{
								return prop.name == name && prop.type == type;
							});

						if (it != regInfo.properties.end())
						{
							switch (type)
							{
								case Wire::ComponentRegistry::PropertyType::Bool:
								{
									bool input;
									VT_DESERIALIZE_PROPERTY(data, input, propNode, false);
									memcpy_s(&componentData[it->offset], sizeof(bool), &input, sizeof(bool));
									break;
								}

								case Wire::ComponentRegistry::PropertyType::Int:
								{
									int32_t input;
									VT_DESERIALIZE_PROPERTY(data, input, propNode, 0);
									memcpy_s(&componentData[it->offset], sizeof(int32_t), &input, sizeof(int32_t));
									break;
								}

								case Wire::ComponentRegistry::PropertyType::UInt:
								{
									uint32_t input;
									VT_DESERIALIZE_PROPERTY(data, input, propNode, 0);
									memcpy_s(&componentData[it->offset], sizeof(uint32_t), &input, sizeof(uint32_t));
									break;
								}

								case Wire::ComponentRegistry::PropertyType::Short:
								{
									int16_t input;
									VT_DESERIALIZE_PROPERTY(data, input, propNode, 0);
									memcpy_s(&componentData[it->offset], sizeof(int16_t), &input, sizeof(int16_t));
									break;
								}

								case Wire::ComponentRegistry::PropertyType::UShort:
								{
									uint16_t input;
									VT_DESERIALIZE_PROPERTY(data, input, propNode, 0);
									memcpy_s(&componentData[it->offset], sizeof(uint16_t), &input, sizeof(uint16_t));
									break;
								}

								case Wire::ComponentRegistry::PropertyType::Char:
								{
									int8_t input;
									VT_DESERIALIZE_PROPERTY(data, input, propNode, 0);
									memcpy_s(&componentData[it->offset], sizeof(int8_t), &input, sizeof(int8_t));
									break;
								}

								case Wire::ComponentRegistry::PropertyType::UChar:
								{
									uint8_t input;
									VT_DESERIALIZE_PROPERTY(data, input, propNode, 0);
									memcpy_s(&componentData[it->offset], sizeof(uint8_t), &input, sizeof(uint8_t));
									break;
								}

								case Wire::ComponentRegistry::PropertyType::Float:
								{
									float input;
									VT_DESERIALIZE_PROPERTY(data, input, propNode, 0.f);
									memcpy_s(&componentData[it->offset], sizeof(float), &input, sizeof(float));
									break;
								}

								case Wire::ComponentRegistry::PropertyType::Double:
								{
									double input;
									VT_DESERIALIZE_PROPERTY(data, input, propNode, 0.0);
									memcpy_s(&componentData[it->offset], sizeof(double), &input, sizeof(double));
									break;
								}

								case Wire::ComponentRegistry::PropertyType::Vector2:
								{
									gem::vec2 input;
									VT_DESERIALIZE_PROPERTY(data, input, propNode, gem::vec2{ 0.f });
									memcpy_s(&componentData[it->offset], sizeof(gem::vec2), &input, sizeof(gem::vec2));
									break;
								}

								case Wire::ComponentRegistry::PropertyType::Vector3:
								{
									gem::vec3 input;
									VT_DESERIALIZE_PROPERTY(data, input, propNode, gem::vec3(0.f));
									memcpy_s(&componentData[it->offset], sizeof(gem::vec3), &input, sizeof(gem::vec3));
									break;
								}

								case Wire::ComponentRegistry::PropertyType::Vector4:
								{
									gem::vec4 input;
									VT_DESERIALIZE_PROPERTY(data, input, propNode, gem::vec4(0.f));
									memcpy_s(&componentData[it->offset], sizeof(gem::vec4), &input, sizeof(gem::vec4));
									break;
								}

								case Wire::ComponentRegistry::PropertyType::String:
								{
									std::string input;
									VT_DESERIALIZE_PROPERTY(data, input, propNode, std::string(""));

									(*(std::string*)&componentData[it->offset]) = input;

									break;
								}

								case Wire::ComponentRegistry::PropertyType::Int64:
								{
									int64_t input;
									VT_DESERIALIZE_PROPERTY(data, input, propNode, 0);
									memcpy_s(&componentData[it->offset], sizeof(int64_t), &input, sizeof(int64_t));
									break;
								}

								case Wire::ComponentRegistry::PropertyType::UInt64:
								{
									uint64_t input;
									VT_DESERIALIZE_PROPERTY(data, input, propNode, 0);
									memcpy_s(&componentData[it->offset], sizeof(uint64_t), &input, sizeof(uint64_t));
									break;
								}

								case Wire::ComponentRegistry::PropertyType::AssetHandle:
								{
									AssetHandle input;
									VT_DESERIALIZE_PROPERTY(data, input, propNode, AssetHandle(0));
									memcpy_s(&componentData[it->offset], sizeof(AssetHandle), &input, sizeof(AssetHandle));
									break;
								}

								case Wire::ComponentRegistry::PropertyType::Color3:
								{
									gem::vec3 input;
									VT_DESERIALIZE_PROPERTY(data, input, propNode, gem::vec3(0.f));
									memcpy_s(&componentData[it->offset], sizeof(gem::vec3), &input, sizeof(gem::vec3));
									break;
								}

								case Wire::ComponentRegistry::PropertyType::Color4:
								{
									gem::vec4 input;
									VT_DESERIALIZE_PROPERTY(data, input, propNode, gem::vec4(0.f));
									memcpy_s(&componentData[it->offset], sizeof(gem::vec4), &input, sizeof(gem::vec4));
									break;
								}

								case Wire::ComponentRegistry::PropertyType::Folder:
								{
									std::filesystem::path input;
									VT_DESERIALIZE_PROPERTY(data, input, propNode, std::filesystem::path(""));

									(*(std::filesystem::path*)&componentData[it->offset]) = input;
									break;
								}

								case Wire::ComponentRegistry::PropertyType::Path:
								{
									std::filesystem::path input;
									VT_DESERIALIZE_PROPERTY(data, input, propNode, std::filesystem::path(""));

									(*(std::filesystem::path*)&componentData[it->offset]) = input;
									break;
								}

								case Wire::ComponentRegistry::PropertyType::EntityId:
								{
									uint32_t input;
									VT_DESERIALIZE_PROPERTY(data, input, propNode, 0);
									memcpy_s(&componentData[it->offset], sizeof(uint32_t), &input, sizeof(uint32_t));
									break;
								}

								case Wire::ComponentRegistry::PropertyType::GUID:
								{
									WireGUID input;
									VT_DESERIALIZE_PROPERTY(data, input, propNode, WireGUID::Null());
									memcpy_s(&componentData[it->offset], sizeof(WireGUID), &input, sizeof(WireGUID));
									break;
								}

								case Wire::ComponentRegistry::PropertyType::Enum:
								{
									uint32_t input;
									VT_DESERIALIZE_PROPERTY(data, input, propNode, 0);
									memcpy_s(&componentData[it->offset], sizeof(uint32_t), &input, sizeof(uint32_t));
									break;
								}

								case Wire::ComponentRegistry::PropertyType::Vector:
								{
									if (!propNode["data"])
									{
										break;
									}

									switch (vectorType)
									{
										case Wire::ComponentRegistry::PropertyType::Bool: DeserializeVector<bool>(componentData, it->offset, propNode["data"], false); break;
										case Wire::ComponentRegistry::PropertyType::Int: DeserializeVector<int32_t>(componentData, it->offset, propNode["data"], 0); break;
										case Wire::ComponentRegistry::PropertyType::UInt: DeserializeVector<uint32_t>(componentData, it->offset, propNode["data"], 0); break;
										case Wire::ComponentRegistry::PropertyType::Short: DeserializeVector<int16_t>(componentData, it->offset, propNode["data"], 0); break;
										case Wire::ComponentRegistry::PropertyType::UShort: DeserializeVector<uint16_t>(componentData, it->offset, propNode["data"], 0); break;
										case Wire::ComponentRegistry::PropertyType::Char: DeserializeVector<int8_t>(componentData, it->offset, propNode["data"], 0); break;
										case Wire::ComponentRegistry::PropertyType::UChar: DeserializeVector<uint8_t>(componentData, it->offset, propNode["data"], 0); break;
										case Wire::ComponentRegistry::PropertyType::Float: DeserializeVector<float>(componentData, it->offset, propNode["data"], 0); break;
										case Wire::ComponentRegistry::PropertyType::Double: DeserializeVector<double>(componentData, it->offset, propNode["data"], 0); break;
										case Wire::ComponentRegistry::PropertyType::Vector2: DeserializeVector<gem::vec2>(componentData, it->offset, propNode["data"], 0); break;
										case Wire::ComponentRegistry::PropertyType::Vector3: DeserializeVector<gem::vec3>(componentData, it->offset, propNode["data"], 0); break;
										case Wire::ComponentRegistry::PropertyType::Vector4: DeserializeVector<gem::vec4>(componentData, it->offset, propNode["data"], 0); break;
										case Wire::ComponentRegistry::PropertyType::String: DeserializeVector<std::string>(componentData, it->offset, propNode["data"], "Null"); break;
										case Wire::ComponentRegistry::PropertyType::Int64: DeserializeVector<int64_t>(componentData, it->offset, propNode["data"], 0); break;
										case Wire::ComponentRegistry::PropertyType::UInt64: DeserializeVector<uint64_t>(componentData, it->offset, propNode["data"], 0); break;
										case Wire::ComponentRegistry::PropertyType::AssetHandle: DeserializeVector<AssetHandle>(componentData, it->offset, propNode["data"], 0); break;
										case Wire::ComponentRegistry::PropertyType::Color3: DeserializeVector<gem::vec3>(componentData, it->offset, propNode["data"], 0); break;
										case Wire::ComponentRegistry::PropertyType::Color4: DeserializeVector<gem::vec4>(componentData, it->offset, propNode["data"], 0); break;
										case Wire::ComponentRegistry::PropertyType::Folder: DeserializeVector<std::filesystem::path>(componentData, it->offset, propNode["data"], "Null"); break;
										case Wire::ComponentRegistry::PropertyType::Path: DeserializeVector<std::filesystem::path>(componentData, it->offset, propNode["data"], "Null"); break;
										case Wire::ComponentRegistry::PropertyType::GUID: DeserializeVector<WireGUID>(componentData, it->offset, propNode["data"], WireGUID::Null()); break;
										case Wire::ComponentRegistry::PropertyType::EntityId: DeserializeVector<Wire::EntityId>(componentData, it->offset, propNode["data"], Wire::NullID); break;
										case Wire::ComponentRegistry::PropertyType::Enum: DeserializeVector<uint32_t>(componentData, it->offset, propNode["data"], 0); break;
									}

									break;
								}
							}
						}
					}
				}
			}
		}

		if (!registry.HasComponent<EntityDataComponent>(entityId))
		{
			registry.AddComponent<EntityDataComponent>(entityId);
		}
	}
}