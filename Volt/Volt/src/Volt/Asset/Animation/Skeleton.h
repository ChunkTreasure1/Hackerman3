#pragma once

#include "Volt/Asset/Asset.h"

#include <GEM/gem.h>

namespace Volt
{
	class Skeleton : public Asset
	{
	public:
		struct Joint
		{
			std::string name;
			int32_t parentIndex = -1;
		};

		Skeleton() = default;
		~Skeleton() override
		{
			myJoints.clear();
			myInverseBindPoses.clear();
		}

		inline const size_t GetJointCount() const { return myJoints.size(); }
		inline const std::vector<gem::mat4> GetInverseBindPoses() const { return myInverseBindPoses; }
		inline const std::vector<Joint>& GetJoints() const { return myJoints; }

		static AssetType GetStaticType() { return AssetType::Skeleton; }
		AssetType GetType() override { return GetStaticType(); };

	private:
		friend class FbxImporter;
		friend class SkeletonImporter;

		std::vector<Joint> myJoints;
		std::vector<gem::mat4> myInverseBindPoses;
		std::string myName = "Skeleton";                 
	};
}