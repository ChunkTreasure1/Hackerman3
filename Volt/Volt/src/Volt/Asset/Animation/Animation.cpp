#include "vtpch.h"
#include "Animation.h"

#include "Volt/Asset/Animation/Skeleton.h"
#include "Volt/Animation/AnimationManager.h"
#include "Volt/Core/Profiling.h"

#include "Volt/Log/Log.h"

#include "Volt/Utility/Math.h"

namespace Volt
{
	const std::vector<gem::mat4> Animation::Sample(float aStartTime, Ref<Skeleton> aSkeleton)
	{
		VT_PROFILE_FUNCTION();

		const float localTime = std::clamp(AnimationManager::globalClock - aStartTime, 0.f, myDuration);

		std::vector<gem::mat4> result;
		result.resize(aSkeleton->GetJointCount(), gem::mat4(1.f));

		const float normalizedTime = localTime / myDuration;

		const size_t frameCount = myFrames.size();
		size_t currentFrameIndex = (size_t)std::floorf((float)frameCount * normalizedTime);
		size_t nextFrameIndex = currentFrameIndex + 1;

		if (nextFrameIndex >= frameCount)
		{
			nextFrameIndex = 1;
		}

		if (currentFrameIndex == frameCount)
		{
			currentFrameIndex = frameCount - 1;
		}

		const float animDelta = 1.f / (float)myFramesPerSecond;
		const float frameTime = localTime / animDelta;
		const float deltaTime = frameTime - (float)currentFrameIndex;

		const Frame& currentFrame = myFrames.at(currentFrameIndex);
		const Frame& nextFrame = myFrames.at(nextFrameIndex);

		const auto& joints = aSkeleton->GetJoints();
		const auto& invBindPoses = aSkeleton->GetInverseBindPoses();

		for (size_t i = 0; i < joints.size(); i++)
		{
			const auto& joint = joints[i];

			gem::mat4 parentTransform = gem::mat4(1.f);

			if (joint.parentIndex >= 0)
			{
				parentTransform = result[joint.parentIndex];
			}

			const gem::mat4 currentLocalTransform = currentFrame.localTransforms[i];
			const gem::mat4 nextLocalTransform = nextFrame.localTransforms[i];

			const gem::mat4 currentGlobalTransform = parentTransform * gem::transpose(currentLocalTransform);
			const gem::mat4 nextGlobalTransform = parentTransform * gem::transpose(nextLocalTransform);

			gem::mat4 resultTransform;

			// Blend
			{
				//gem::vec3 cP, cR, cS;
				//gem::vec3 nP, nR, nS;

				//Math::DecomposeTransform(currentGlobalTransform, cP, cR, cS);
				//Math::DecomposeTransform(nextGlobalTransform, nP, nR, nS);

				//gem::vec3 rP = gem::lerp(cP, nP, deltaTime);
				//gem::vec3 rR = gem::nlerp(cR, nR, deltaTime);
				//gem::vec3 rS = gem::lerp(cS, nS, deltaTime);

				//resultTransform = gem::translate(gem::mat4(1.f), rP) *
				//	gem::rotate(gem::mat4(1.f), rR.x, gem::vec3{ 1.f, 0.f, 0.f }) *
				//	gem::rotate(gem::mat4(1.f), rR.y, gem::vec3{ 0.f, 1.f, 0.f }) *
				//	gem::rotate(gem::mat4(1.f), rR.z, gem::vec3{ 0.f, 0.f, 1.f }) *
				//	gem::scale(gem::mat4(1.f), rS);

				resultTransform = Math::Lerp(currentGlobalTransform, nextGlobalTransform, deltaTime);
			}

			gem::mat4 resultT = resultTransform;
			result[i] = resultT;
		}

		for (size_t i = 0; i < result.size(); i++)
		{
			result[i] = result[i] * invBindPoses[i];
		}

		return result;
	}
}

