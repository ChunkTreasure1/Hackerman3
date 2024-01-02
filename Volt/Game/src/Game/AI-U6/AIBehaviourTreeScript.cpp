#include "AIBehaviourTreeScript.h"
#include "Volt/Input/Input.h"
#include "Volt/Input/KeyCodes.h"
#include "Volt/Utility/Random.h"
#include "Volt/Physics/PhysicsActor.h"
#include "Volt/Physics/PhysicsScene.h"
#include "Volt/Physics/Physics.h"
#include "Volt/Asset/Prefab.h"
#include "Volt/Asset/AssetManager.h"
#include "../AI-U5/PollingStationU5.h"
#include "../AI-U5/StateMachineActor.h"
#include "../AI-U5/DecisionTreeActor.h"
#include "../AI-U5/HealthWellScript.h"

#include <Volt/Rendering/Renderer.h>


VT_REGISTER_SCRIPT(AIBehaviourTreeScript)

AIBehaviourTreeScript::AIBehaviourTreeScript(const Volt::Entity& aEntity)
	:ScriptBase(aEntity)
{
	myEntity.GetComponent<AIU5HealthComponent>().currentHealth = myEntity.GetComponent<AIU5HealthComponent>().health;
}

void AIBehaviourTreeScript::OnCollisionEnter(Volt::Entity entity)
{
	if (entity.HasScript("ProjectileScript"))
	{
		myEntity.GetComponent<AIU5HealthComponent>().currentHealth -= 1;
		myEntity.GetScene()->RemoveEntity(entity);
		if (myEntity.GetComponent<AIU5HealthComponent>().currentHealth <= 0)
		{
			myEntity.GetScene()->RemoveEntity(myEntity);
		}
		return;
	}

}


void ShootBullet(const Volt::Entity aEntity, const gem::vec3& direction, const float speed)
{
	auto entity = aEntity.GetScene()->CreateEntity();
	entity.SetPosition(aEntity.GetPosition());

	// Mesh
	{
		auto& meshComp = entity.AddComponent<Volt::MeshComponent>();
		meshComp.handle = Volt::AssetManager::GetAsset<Volt::Mesh>("Assets/Meshes/Primitives/Sphere.vtmesh")->handle;
	}

	// Collider
	{
		Volt::RigidbodyComponent ridigComp{};
		ridigComp.disableGravity = true;
		ridigComp.bodyType = Volt::BodyType::Dynamic;
		ridigComp.layerId = 4;
		ridigComp.collisionType = Volt::CollisionDetectionType::Continuous;

		entity.AddComponent<Volt::SphereColliderComponent>();
		entity.AddComponent<Volt::RigidbodyComponent>(&ridigComp);

		entity.SetPosition(aEntity.GetPosition() + direction * 300.f);
	}

	entity.AddScript("BulletScript");
	entity.GetPhysicsActor()->AddForce(direction * speed, Volt::ForceMode::Force);
}

bool AIBehaviourTreeScript::Shoot()
{

	const auto& comp = myEntity.GetComponent<AIU5DecisionActorComponent>();

	const auto& targetPos = PollingStationU5::Get().PollStateTargetPosition();
	const gem::vec3 direction = gem::normalize(targetPos - myEntity.GetPosition());

	const gem::vec3 rotTo = gem::eulerAngles(gem::fromTo(direction, myEntity.GetForward()));

	if (rotTo.y + myEntity.GetRotation().y < myEntity.GetRotation().y - 0.01)
	{
		myEntity.GetPhysicsActor()->SetAngularVelocity({ 0.f, comp.turningSpeed * -1.f, 0.f });
		myEntity.GetPhysicsActor()->SetLinearVelocity({ 0.f });
		return false;
	}
	else if (rotTo.y + myEntity.GetRotation().y > myEntity.GetRotation().y + 0.01)
	{
		myEntity.GetPhysicsActor()->SetAngularVelocity({ 0.f, comp.turningSpeed, 0.f });
		myEntity.GetPhysicsActor()->SetLinearVelocity({ 0.f });
		return false;
	}
	else
	{
		myEntity.GetPhysicsActor()->SetAngularVelocity({ 0.f });

		const float distance = gem::distance(myEntity.GetPosition(), targetPos);
		if (distance > comp.shootDistance)
		{
			const gem::vec3 dir = gem::normalize(direction);
			myEntity.GetPhysicsActor()->SetLinearVelocity(gem::vec3{ dir.x, 0.f, dir.z } *comp.speed);
			return false;
		}
		else
		{
			myEntity.GetPhysicsActor()->SetLinearVelocity({ 0.f });
		}

		const float timeSinceShot = std::any_cast<float>(blackboard.at("TimeSinceShot"));
		if (timeSinceShot > 1.f / comp.fireRate)
		{
			ShootBullet(myEntity, direction, 200000.f + comp.speed);
			blackboard["TimeSinceShot"] = 0.f;
			return true;
		}
		else
		{
			blackboard["TimeSinceShot"] = timeSinceShot + myDT;
		}

		return false;
	}
}

void SetVelocity(Volt::Entity aEntity, const gem::vec3& direction)
{
	const auto& comp = aEntity.GetComponent<AIU5DecisionActorComponent>();
	aEntity.GetPhysicsActor()->SetLinearVelocity(direction * comp.speed);

	const gem::vec3 rot = gem::eulerAngles(gem::quatLookAt(gem::vec3{ direction.x, 0.f, direction.z }, { 0.f, 1.f, 0.f }));
	aEntity.SetRotation(rot);
}

void AIBehaviourTreeScript::Wander()
{
	const auto& comp = myEntity.GetComponent<AIU5DecisionActorComponent>();
	Volt::RaycastHit forwardHit;

	const gem::vec3 directionLeft = gem::normalize(gem::rotate(gem::quat(gem::vec3{ 0.f, -gem::radians(comp.whiskarAngle), 0.f }), myEntity.GetForward()));
	const gem::vec3 directionRight = gem::normalize(gem::rotate(gem::quat(gem::vec3{ 0.f, gem::radians(comp.whiskarAngle), 0.f }), myEntity.GetForward()));

	Volt::RaycastHit leftHit;
	const bool rightWisker = Volt::Physics::GetScene()->Raycast(myEntity.GetPosition(), directionLeft, comp.maxSideDistance, &leftHit, { 0 });

	Volt::RaycastHit rightHit;
	const bool leftWisker = Volt::Physics::GetScene()->Raycast(myEntity.GetPosition(), directionRight, comp.maxSideDistance, &rightHit, { 0 });

	myEntity.GetPhysicsActor()->SetAngularVelocity(0.f);

	if (myEntity.GetPhysicsActor()->GetLinearVelocity() == 0.f)
	{
		myEntity.GetPhysicsActor()->SetLinearVelocity(myEntity.GetForward() * comp.speed);
	}

	const gem::vec3 currentVelocity = myEntity.GetPhysicsActor()->GetLinearVelocity();
	const gem::vec3 currentDirection = gem::normalize(currentVelocity);
	if (Volt::Physics::GetScene()->Raycast(myEntity.GetPosition(), currentDirection, 10000.f, &forwardHit, { 0 }))
	{
		if (forwardHit.distance < 1000.f)
		{
			const gem::vec3 reflectedDir = gem::reflect(currentDirection, forwardHit.normal);
			SetVelocity(myEntity, gem::vec3(reflectedDir.x, 0.f, reflectedDir.z));
		}
	}

	if (rightWisker && rightHit.distance < comp.maxSideDistance)
	{
		const gem::vec3 reflectedDir = gem::reflect(directionRight, rightHit.normal);
		SetVelocity(myEntity, gem::vec3(reflectedDir.x, 0.f, reflectedDir.z));
	}
	if (leftWisker && leftHit.distance < comp.maxSideDistance)
	{
		const gem::vec3 reflectedDir = gem::reflect(directionLeft, leftHit.normal);
		SetVelocity(myEntity, gem::vec3(reflectedDir.x, 0.f, reflectedDir.z));
	}

	Volt::Renderer::SubmitLine(myEntity.GetPosition(), myEntity.GetPosition() + currentDirection * 1000 + gem::vec3{ 0.f, 100.f, 0.f });
	Volt::Renderer::SubmitLine(myEntity.GetPosition() + gem::vec3{ 0.f, 100.f, 0.f }, myEntity.GetPosition() + directionRight * comp.maxSideDistance + gem::vec3{ 0.f, 100.f, 0.f });
	Volt::Renderer::SubmitLine(myEntity.GetPosition() + gem::vec3{ 0.f, 100.f, 0.f }, myEntity.GetPosition() + directionLeft * comp.maxSideDistance + gem::vec3{ 0.f, 100.f, 0.f });
}

bool AIBehaviourTreeScript::CanSeeEnemy()
{
	const auto& targetPos = PollingStationU5::Get().PollStateTargetPosition();

	const gem::vec3 direction = gem::normalize(targetPos - myEntity.GetPosition());
	const gem::vec3 origin = myEntity.GetPosition();

	Volt::RaycastHit hit;
	if (Volt::Physics::GetScene()->Raycast(origin, direction, 10000.f, &hit, { 0, 1 }))
	{
		Volt::Entity hitEntity = { hit.hitEntity, myEntity.GetScene() };
		if (hitEntity.HasComponent<AIU5StateActorComponent>() && !hitEntity.GetScript<StateMachineActor>("StateMachineActor")->IsDead())
		{
			return true;
		}
	}

	return false;
}

bool AIBehaviourTreeScript::IsInRange()
{
	const auto& comp = myEntity.GetComponent<AIU5DecisionActorComponent>();

	const auto& targetPos = PollingStationU5::Get().PollStateTargetPosition();
	const gem::vec3 direction = gem::normalize(targetPos - myEntity.GetPosition());

	const gem::vec3 rotTo = gem::eulerAngles(gem::fromTo(direction, myEntity.GetForward()));

	if (rotTo.y + myEntity.GetRotation().y < myEntity.GetRotation().y - 0.01)
	{
		myEntity.GetPhysicsActor()->SetAngularVelocity({ 0.f, comp.turningSpeed * -1.f, 0.f });
		myEntity.GetPhysicsActor()->SetLinearVelocity({ 0.f });
		return false;
	}
	else if (rotTo.y + myEntity.GetRotation().y > myEntity.GetRotation().y + 0.01)
	{
		myEntity.GetPhysicsActor()->SetAngularVelocity({ 0.f, comp.turningSpeed, 0.f });
		myEntity.GetPhysicsActor()->SetLinearVelocity({ 0.f });
		return false;
	}
	else
	{
		return true;
	}

}

bool AIBehaviourTreeScript::LowHealth()
{
	const auto& health = myEntity.GetComponent<AIU5HealthComponent>().currentHealth;
	if (!myIsOnHealthWell && health < 40.f)
	{
		return true;
	}

	if (myIsOnHealthWell && health < 80.f)
	{
		return true;
	}

	return false;
}

bool AIBehaviourTreeScript::IsDead()
{
	const auto& health = myEntity.GetComponent<AIU5HealthComponent>().currentHealth;
	return health <= 0;
}

bool AIBehaviourTreeScript::IsOnHealthWell()
{
	return myIsOnHealthWell;
}

bool AIBehaviourTreeScript::UpdateRespawn()
{
	myEntity.GetComponent<Volt::TransformComponent>().visible = false;

	myDeathTimer -= myDT;
	if (myDeathTimer <= 0.f)
	{
		myEntity.GetComponent<AIU5HealthComponent>().currentHealth = myEntity.GetComponent<AIU5HealthComponent>().health;
		myEntity.GetComponent<Volt::TransformComponent>().visible = true;

		myEntity.GetPhysicsActor()->SetAngularVelocity(0.f);
		myEntity.GetPhysicsActor()->SetLinearVelocity(0.f);

		myEntity.SetPosition(PollingStationU5::Get().PollDecisionRespawnPosition());
		myEntity.SetRotation(myStartRot);
		myDeathTimer = 5.f;
		myIsOnHealthWell = false;
		return false;
	}

	return true;
}

void AIBehaviourTreeScript::GoToHealthWell()
{
	const gem::vec3 closestHealthWell = GetClosestHealthWell().GetPosition();
	const float distance = gem::distance(myEntity.GetPosition(), closestHealthWell);

	const auto& comp = myEntity.GetComponent<AIU5DecisionActorComponent>();
	Volt::RaycastHit forwardHit;

	myEntity.GetPhysicsActor()->SetAngularVelocity(0.f);

	if (myEntity.GetPhysicsActor()->GetLinearVelocity() == 0.f)
	{
		myEntity.GetPhysicsActor()->SetLinearVelocity(myEntity.GetForward() * comp.speed);
	}

	const gem::vec3 currentVelocity = myEntity.GetPhysicsActor()->GetLinearVelocity();
	gem::vec3 currentDirection = gem::normalize(currentVelocity);

	const bool hitWall = Volt::Physics::GetScene()->Raycast(myEntity.GetPosition(), gem::normalize(closestHealthWell - myEntity.GetPosition()), gem::distance(closestHealthWell, myEntity.GetPosition()), &forwardHit, { 0 });

	if (!hitWall)
	{
		currentDirection = gem::normalize(closestHealthWell - myEntity.GetPosition());
		SetVelocity(myEntity, gem::vec3(currentDirection.x, 0.f, currentDirection.z));
	}

	if (hitWall && Volt::Physics::GetScene()->Raycast(myEntity.GetPosition(), currentDirection, 10000.f, &forwardHit, { 0 }))
	{
		if (forwardHit.distance < 1000.f)
		{
			const gem::vec3 reflectedDir = gem::reflect(currentDirection, forwardHit.normal);
			SetVelocity(myEntity, gem::vec3(reflectedDir.x, 0.f, reflectedDir.z));
		}
	}
}

Volt::Entity AIBehaviourTreeScript::GetClosestHealthWell()
{
	float closestWellDist = FLT_MAX;
	Volt::Entity closestWell;

	myEntity.GetScene()->GetRegistry().ForEach<HealthWellComponent>([&](Wire::EntityId id, const HealthWellComponent& comp)
		{
			Volt::Entity ent{ id, myEntity.GetScene() };
			const float distance = gem::distance(myEntity.GetPosition(), ent.GetPosition());
			const bool isAvailiable = ent.GetScript<HealthWellScript>("HealthWellScript")->GetPlayer() == Volt::Entity{} || ent.GetScript<HealthWellScript>("HealthWellScript")->GetPlayer() == myEntity;

			if (distance < closestWellDist && isAvailiable)
			{
				closestWell = ent;
				closestWellDist = distance;
			}
		});

	return closestWell;
}

void AIBehaviourTreeScript::OnTriggerEnter(Volt::Entity entity, bool isTrigger)
{
	if (entity.HasComponent<HealthWellComponent>())
	{
		myIsOnHealthWell = true;
	}
}

void AIBehaviourTreeScript::OnTriggerExit(Volt::Entity entity, bool isTrigger)
{
	if (entity.HasComponent<HealthWellComponent>())
	{
		myIsOnHealthWell = false;
	}
}


void AIBehaviourTreeScript::OnStart()
{
	auto& healthComp = myEntity.GetComponent<AIU5HealthComponent>();
	myHealth = healthComp.health;
	myDeathTimer = 5.f;
	myBT.Init(myEntity);

	PollingStationU5::Get().SetDecisionRespawn(myStartPos);

	myEntity.GetScene()->GetRegistry().ForEach<AIU5StateActorComponent>([&](Wire::EntityId id, const AIU5StateActorComponent& scriptComp)
		{
			myEnemy = { id, myEntity.GetScene() };
			return;
		});

	blackboard["TimeSinceShot"] = 10.f;
	blackboard["IsTurning"] = false;
}

void AIBehaviourTreeScript::OnUpdate(float aDeltaTime)
{
	myDT = aDeltaTime;
	myBT.Update();


	if (Volt::Input::IsKeyPressed(VT_KEY_T))
	{
		testBool = !testBool;
	}

	if (myShootTimer > 0)
	{
		myShootTimer -= aDeltaTime;
	}

	if (myReloadTimer > 0)
	{
		myReloadTimer -= aDeltaTime;
	}

	if (myNewMoveDirTimer > 0)
	{
		myNewMoveDirTimer -= aDeltaTime;
	}

	if (myHealingTimer > 0)
	{
		myHealingTimer -= aDeltaTime;
	}
}
