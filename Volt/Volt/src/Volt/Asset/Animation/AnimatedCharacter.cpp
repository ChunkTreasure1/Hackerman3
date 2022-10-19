#include "vtpch.h"
#include "AnimatedCharacter.h"

#include "Volt/Asset/Animation/Animation.h"
#include "Volt/Log/Log.h"

namespace Volt
{
	const std::vector<gem::mat4> AnimatedCharacter::SampleAnimation(uint32_t index, float aStartTime) const
	{
		if (myAnimations.find(index) == myAnimations.end())
		{
			VT_CORE_WARN("Trying to access animation at invalid index {0}!", index);
			return std::vector<gem::mat4>{};
		}

		return myAnimations.at(index)->Sample(aStartTime, mySkeleton);
	}

	const float AnimatedCharacter::GetAnimationDuration(uint32_t index) const
	{
		if (myAnimations.find(index) == myAnimations.end())
		{
			VT_CORE_WARN("Trying to access animation at invalid index {0}!", index);
			return 0.f;
		}

		return myAnimations.at(index)->GetDuration();
	}
}