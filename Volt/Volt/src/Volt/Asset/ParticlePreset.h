#pragma once
#include "Volt/Asset/Asset.h"
#include "GEM/gem.h"

namespace Volt
{
	class ParticlePreset : public Asset
	{
	public:
		bool fade;
		float emissiveness;
		float distance;
		float velocity;
		float intensity;
		std::string shader;
		float spread;
		gem::vec3 direction;
		gem::vec3 gravity;
		gem::vec4 color;
		int poolSize;

		static AssetType GetStaticType() { return AssetType::ParticlePreset; }
		virtual AssetType GetType() override { return GetStaticType(); }
	};
}