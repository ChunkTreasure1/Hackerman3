#pragma once
#include "Volt/Core/Base.h"
#include "Volt/Scene/Scene.h"
#include "NavMeshData.h"

namespace Volt
{
	class NavigationsSystem
	{
	public:
		NavigationsSystem(Ref<Scene>& aScene);
		~NavigationsSystem();

		static void SetNavMesh(const NavMesh& aNavMesh) { myCurrentNavMesh = aNavMesh; };
		static const NavMesh& GetNavMesh() { return myCurrentNavMesh; }
		void OnRuntimeUpdate(float aTimestep);
		void OnRuntimeStop();

	private:
		inline static NavMesh myCurrentNavMesh;
		Ref<Scene>& myCurrentScene;

		bool test = false;
	};
}