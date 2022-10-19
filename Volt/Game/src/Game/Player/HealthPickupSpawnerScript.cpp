#include "HealthPickupSpawnerScript.h"
#include "HealthPickupScript.h"
#include "Volt/Asset/AssetManager.h"
#include "Volt/Components/PhysicsComponents.h"
#include "Volt/Input/Input.h"
#include "Volt/Input/KeyCodes.h"

VT_REGISTER_SCRIPT(HealthPickupSpawnerScript)

HealthPickupSpawnerScript::HealthPickupSpawnerScript(const Volt::Entity& aEntity)
	:ScriptBase(aEntity)
{
}

void HealthPickupSpawnerScript::OnAwake()
{
	for (size_t i = 0; i < 50; i++)
	{
		Volt::Entity ent = myEntity.GetScene()->CreateEntity();
		auto& meshComp = ent.AddComponent<Volt::MeshComponent>();
		meshComp.handle = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/Meshes/Primitives/Sphere.vtmesh");
		meshComp.overrideMaterial = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/Materials/M_BloomTest.vtmat");

		ent.SetPosition(myEntity.GetPosition());

		ent.AddScript("HealthPickupScript");
	}

}

void HealthPickupSpawnerScript::OnUpdate(float aDeltaTime)
{
	if (Volt::Input::IsKeyPressed(VT_KEY_8))
	{
		for (size_t i = 0; i < 100; i++)
		{
			Volt::Entity ent = myEntity.GetScene()->CreateEntity();
			auto& meshComp = ent.AddComponent<Volt::MeshComponent>();
			meshComp.handle = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/Meshes/Primitives/Sphere.vtmesh");

			ent.SetPosition(myEntity.GetPosition());

			ent.AddScript("HealthPickupScript");
		}
	}
}
