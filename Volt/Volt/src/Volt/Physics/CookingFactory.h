#pragma once

#include "Volt/Core/Buffer.h"
#include "Volt/Components/PhysicsComponents.h"
#include "Volt/Physics/PhysicsEnums.h"

#include <GEM/gem.h>
#include <PhysX/PxPhysicsAPI.h>
	
namespace Volt
{
	struct MeshColliderData
	{
		Buffer data{};
		gem::mat4 transform;
	};

	struct MeshColliderCacheData
	{
		std::vector<MeshColliderData> colliderData;
	};

	class Mesh;
	class CookingFactory
	{
	public:
		static void Initialize();
		static void Shutdown();

		static CookingResult CookMesh(MeshColliderComponent& colliderComp, bool invalidateOld, std::vector<MeshColliderData>& outData);

	private:
		struct CookingSettings
		{
			int32_t subMeshIndex = -1;
		};

		struct Data
		{
			Data(const physx::PxTolerancesScale& scale)
				: cookingParams(scale)
			{}

			physx::PxCooking* cookingSDK = nullptr;
			physx::PxCookingParams cookingParams;
		};

		static CookingResult CookConvexMesh(Ref<Mesh> mesh, uint32_t subMeshIndex, MeshColliderData& outData);
		static CookingResult CookTriangleMesh(Ref<Mesh> srcMesh, uint32_t subMeshIndex, MeshColliderData& outData);

		inline static Scope<Data> myCookingData;
	};
}