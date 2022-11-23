#include "SeekController.h"

#include "PollingStation.h"

#include "Volt/Components/Components.h"
#include "Volt/Components/PhysicsComponents.h"
#include "Volt/Utility/Random.h"
#include "Volt/Physics/PhysicsScene.h"
#include "Volt/Physics/Physics.h"

VT_REGISTER_SCRIPT(SeekController);

SeekController::SeekController(Volt::Entity entity)
	: Volt::ScriptBase(entity)
{}

void SeekController::OnStart()
{}

void SeekController::OnUpdate(float aDeltaTime)
{
	const gem::vec3 targetPosition = PollingStation::Get().PollTargetPosition();
	const gem::vec3 direction = gem::normalize(targetPosition - myEntity.GetPosition());

	const auto& seekComponent = myEntity.GetComponent<SeekControllerComponent>();

	myEntity.SetPosition(myEntity.GetPosition() + direction * seekComponent.speed * aDeltaTime);
	myEntity.SetRotation(gem::eulerAngles(gem::quatLookAt(direction, { 0.f, 1.f, 0.f })));

	auto currPos = myEntity.GetPosition();
	if (currPos.x > 2500.f)
	{
		myEntity.SetPosition({ -2500.f, currPos.y, currPos.z });
	}
	else if (currPos.x < -2500.f)
	{
		myEntity.SetPosition({ 2500.f, currPos.y, currPos.z });
	}
	else if (currPos.z > 2500.f)
	{
		myEntity.SetPosition({ currPos.x, currPos.y, -2500.f });
	}
	else if (currPos.z < -2500.f)
	{
		myEntity.SetPosition({ currPos.x, currPos.y, 2500.f });
	}

	if (myExplode)
	{
		CreateColl();
	}
}

void SeekController::OnTriggerEnter(Volt::Entity entity, bool isTrigger)
{
	myExplode = true;
}

void SeekController::CreateColl()
{
	auto entity = myEntity.GetScene()->InstantiateSplitMesh("Assets/Meshes/ShatterTest/SM_ShatterTest.vtmesh");
	auto& relComp = entity.GetComponent<Volt::RelationshipComponent>();

	entity.SetPosition(myEntity.GetPosition() );

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
		actor->AddForce({ Volt::Random::Float(-1000, 1000), Volt::Random::Float(40, 1000), Volt::Random::Float(-1000, 1000) }, Volt::ForceMode::Impulse);
	}

	auto children = relComp.Children;
	for (const auto& child : children)
	{
		Volt::Entity childEnt = Volt::Entity{ child, myEntity.GetScene() };
		myEntity.GetScene()->UnparentEntity(childEnt);
	}

	myExplode = false;
}
