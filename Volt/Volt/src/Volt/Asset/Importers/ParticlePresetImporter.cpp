#include "vtpch.h"
#include "ParticlePresetImporter.h"
#include "Volt/Log/Log.h"

#include <yaml-cpp/yaml.h>
#include "Volt/Asset/ParticlePreset.h"
#include "Volt/Utility/SerializationMacros.h"
#include "Volt/Utility/YAMLSerializationHelpers.h"

bool Volt::ParticlePresetImporter::Load(const std::filesystem::path& path, Ref<Asset>& asset) const
{
	if (!std::filesystem::exists(path)) [[unlikely]]
	{
		VT_CORE_ERROR("File {0} not found!", path.string().c_str());
		asset->SetFlag(AssetFlag::Missing, true);
		return false;
	}

	std::ifstream file(path);
	if (!file.is_open()) [[unlikely]]
	{
		VT_CORE_ERROR("Failed to open file: {0}!", path.string().c_str());
		asset->SetFlag(AssetFlag::Invalid, true);
		return false;
	}

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
		asset->SetFlag(AssetFlag::Invalid, true);
		return false;
	}

	YAML::Node emitterNode = root["Emitter"];

	asset = CreateRef<ParticlePreset>();
	Ref<ParticlePreset> p = std::reinterpret_pointer_cast<ParticlePreset>(asset);

	p->fade = emitterNode["fade"].as<bool>();
	p->emissiveness = emitterNode["emissiveness"].as<float>();
	p->distance = emitterNode["distance"].as<float>();
	p->velocity = emitterNode["velocity"].as<float>();
	p->intensity = emitterNode["intensity"].as<float>();
	p->shader = emitterNode["shader"].as<std::string>();
	p->spread = emitterNode["spread"].as<float>();
	p->direction = emitterNode["direction"].as<gem::vec3>();
	p->gravity = emitterNode["gravity"].as<gem::vec3>();
	p->color = emitterNode["color"].as<gem::vec4>();
	p->poolSize = emitterNode["poolSize"].as<int>();

	return true;
}

void Volt::ParticlePresetImporter::Save(const Ref<Asset>& asset) const
{
	Ref<ParticlePreset> p = std::reinterpret_pointer_cast<ParticlePreset>(asset);
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Emitter" << YAML::Value;
	{
		out << YAML::BeginMap;
		VT_SERIALIZE_PROPERTY(fade, p->fade, out);
		VT_SERIALIZE_PROPERTY(emissiveness, p->emissiveness, out);
		VT_SERIALIZE_PROPERTY(distance, p->distance, out);
		VT_SERIALIZE_PROPERTY(velocity, p->velocity, out);
		VT_SERIALIZE_PROPERTY(intensity, p->intensity, out);
		VT_SERIALIZE_PROPERTY(shader, p->shader, out);
		VT_SERIALIZE_PROPERTY(spread, p->spread, out);
		VT_SERIALIZE_PROPERTY(direction, p->direction, out);
		VT_SERIALIZE_PROPERTY(gravity, p->gravity, out);
		VT_SERIALIZE_PROPERTY(color, p->color, out);
		VT_SERIALIZE_PROPERTY(poolSize, p->poolSize, out);
		out << YAML::EndMap;
	}
	out << YAML::EndMap;
	std::ofstream fout(asset->path);
	fout << out.c_str();
	fout.close();
}

