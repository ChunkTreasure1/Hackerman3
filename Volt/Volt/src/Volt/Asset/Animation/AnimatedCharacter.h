#pragma once

#include "Volt/Asset/Asset.h"

#include <gem/gem.h>

#include <map>

namespace Volt
{
	class Skeleton;
	class Mesh;
	class Animation;

	class AnimatedCharacter : public Asset
	{
	public:
		AnimatedCharacter() = default;
		~AnimatedCharacter() override = default;

		const std::vector<gem::mat4> SampleAnimation(uint32_t index, float aStartTime) const;
		const float GetAnimationDuration(uint32_t index) const;
		inline const std::map<uint32_t, Ref<Animation>>& GetAnimations() const { return myAnimations; }
		inline const size_t GetAnimationCount() const { return myAnimations.size(); }

		inline void SetSkeleton(Ref<Skeleton> skeleton) { mySkeleton = skeleton; }
		inline void SetSkin(Ref<Mesh> skin) { mySkin = skin; }
		inline void SetAnimation(uint32_t index, Ref<Animation> anim) { myAnimations[index] = anim; }

		inline Ref<Mesh> GetSkin() const { return mySkin; }

		static AssetType GetStaticType() { return AssetType::AnimatedCharacter; }
		AssetType GetType() override { return GetStaticType(); };

	private:
		friend class AnimatedCharacterImporter;

		Ref<Skeleton> mySkeleton;
		Ref<Mesh> mySkin;
	
		std::map<uint32_t, Ref<Animation>> myAnimations;
	};
}