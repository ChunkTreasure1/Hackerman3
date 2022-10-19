#pragma once

#include "Volt/Asset/Asset.h"

#include <GEM/gem.h>

namespace Volt
{
	class Skeleton;
	class Animation : public Asset
	{
	public:
		struct Frame
		{
			std::vector<gem::mat4> localTransforms;
		};

		const std::vector<gem::mat4> Sample(float aStartTime, Ref<Skeleton> aSkeleton);
		inline const float GetDuration() const { return myDuration; }

		static AssetType GetStaticType() { return AssetType::Animation; }
		AssetType GetType() override { return GetStaticType(); };

	private:
		friend class FbxImporter;
		friend class AnimationImporter;

		std::vector<Frame> myFrames;
		uint32_t myFramesPerSecond = 0;
		float myDuration = 0.f;
	};
}