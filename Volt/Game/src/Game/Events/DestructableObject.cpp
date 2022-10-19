#include "DestructibleObject.h"
#include <Volt/Components/PhysicsComponents.h>
#include <Volt/Components/Components.h>

#include <Volt/Asset/AssetManager.h>

#include <Volt/Physics/PhysicsActor.h>
#include <Volt/Physics/PhysicsScene.h>
#include <Volt/Physics/Physics.h>

#include <Volt/Utility/Random.h>
#include <Volt/Events/KeyEvent.h>

DestructableObject::DestructableObject(Volt::Entity entity)
	: Volt::ScriptBase(entity)
{
}

void DestructableObject::OnTriggerEnter(Volt::Entity entity, bool isTrigger)
{

	{
		ExpolodeObject(entity.GetRight());
	}
}

void DestructableObject::ExpolodeObject(gem::vec3 aForceDir)
{
	//TO DO: not hardcoded shatter mesh!
	auto entity = myEntity.GetScene()->InstantiateSplitMesh("Assets/Meshes/ShatterTest/SM_ShatterTest.vtmesh");
	
	auto& relComp = entity.GetComponent<Volt::RelationshipComponent>();

	gem::vec3 currentPos = myEntity.GetPosition();
	entity.SetPosition(currentPos);

	for (const auto& child : relComp.Children)
	{
		Volt::Entity childEnt = Volt::Entity{ child, myEntity.GetScene() };

		auto& meshCollComp = childEnt.AddComponent<Volt::MeshColliderComponent>();
		meshCollComp.colliderMesh = childEnt.GetComponent<Volt::MeshComponent>().handle;
		meshCollComp.subMeshIndex = childEnt.GetComponent<Volt::MeshComponent>().subMeshIndex;
		meshCollComp.isConvex = true;

		Volt::RigidbodyComponent rigidInit{};
		rigidInit.bodyType = Volt::BodyType::Dynamic;
		auto& rigidComp = childEnt.AddComponent<Volt::RigidbodyComponent>(&rigidInit);
		auto actor = Volt::Physics::GetScene()->GetActor(childEnt);
		actor->AddForce({ Volt::Random::Float(-10, 10), Volt::Random::Float(-10, 10), Volt::Random::Float(-10, 10) }, Volt::ForceMode::Impulse);
	}

	auto children = relComp.Children;
	for (const auto& child : children)
	{
		Volt::Entity childEnt = Volt::Entity{ child, myEntity.GetScene() };
		myEntity.GetScene()->UnparentEntity(childEnt);
	}

	Volt::Entity healthSpawner = myEntity.GetScene()->CreateEntity();
	healthSpawner.SetPosition({myEntity.GetPosition().x, 0, myEntity.GetPosition().z});
	healthSpawner.AddScript("HealthPickupSpawnerScript");

	hasExploded = true;
	myEntity.GetScene()->RemoveEntity(myEntity, 0.1f);
}

VT_REGISTER_SCRIPT(DestructableObject);
