#include "DecisionTreeActor.h"

#include "PollingStationU5.h"
#include "HealthWellScript.h"
#include "StateMachineActor.h"

#include <Volt/Components/Components.h>
#include <Volt/Components//PhysicsComponents.h>
#include <Volt/Rendering/Renderer.h>
#include <Volt/Physics/Physics.h>
#include <Volt/Physics/PhysicsScene.h>

#include <Volt/Utility/Random.h>
#include <Volt/Asset/AssetManager.h>
#include <iostream>

VT_REGISTER_SCRIPT(DecisionTreeActor);

DecisionTreeActor::DecisionTreeActor(Volt::Entity entity)
	: Volt::ScriptBase(entity)
{
	myEntity.GetComponent<AIU5HealthComponent>().currentHealth = myEntity.GetComponent<AIU5HealthComponent>().health;
}

void DecisionTreeActor::OnStart()
{
	myStartPos = myEntity.GetPosition();
	myStartRot = myEntity.GetRotation();

	PollingStationU5::Get().SetDecisionRespawn(myStartPos);

	blackboard["TimeSinceShot"] = 10.f;
	blackboard["IsTurning"] = false;

	SetVelocity({ Volt::Random::Float(-1.f, 1.f), 0.f, Volt::Random::Float(-1.f, 1.f) });
}

void DecisionTreeActor::OnUpdate(float aDeltaTime)
{
	myIsDead = myEntity.GetComponent<AIU5HealthComponent>().currentHealth <= 0;

	if (myIsDead)
	{
		myEntity.GetComponent<Volt::TransformComponent>().visible = false;

		myDeathTimer -= aDeltaTime;
		if (myDeathTimer <= 0.f)
		{
			myEntity.GetComponent<AIU5HealthComponent>().currentHealth = myEntity.GetComponent<AIU5HealthComponent>().health;
			myEntity.GetComponent<Volt::TransformComponent>().visible = true;

			myEntity.GetPhysicsActor()->SetAngularVelocity(0.f);
			myEntity.GetPhysicsActor()->SetLinearVelocity(0.f);

			myEntity.SetPosition(PollingStationU5::Get().PollDecisionRespawnPosition());
			myEntity.SetRotation(myStartRot);
			myDeathTimer = 5.f;
			myIsDead = false;
			myIsOnHealthWell = false;
		}

		return;
	}

	if (!IsHurt())
	{
		if (CanSeeBish())
		{
			Shoot(aDeltaTime);
		}
		else
		{
			Search();
		}
	}
	else
	{
		if (myIsOnHealthWell)
		{
			myEntity.GetPhysicsActor()->SetAngularVelocity({ 0.f, 0.f, 0.f });
			myEntity.GetPhysicsActor()->SetLinearVelocity({ 0.f });

			if (CanSeeBish())
			{
				Shoot(aDeltaTime);
			}
		}
		else
		{
			GoToHealingPool();
		}
	}


}

bool DecisionTreeActor::IsHurt()
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

bool DecisionTreeActor::IsDead()
{
	return myIsDead;
}

bool DecisionTreeActor::CanSeeBish()
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


void DecisionTreeActor::Shoot(float aDeltaTime)
{
	const auto& comp = myEntity.GetComponent<AIU5DecisionActorComponent>();

	const auto& targetPos = PollingStationU5::Get().PollStateTargetPosition();
	const gem::vec3 direction = gem::normalize(targetPos - myEntity.GetPosition());

	const gem::vec3 rotTo = gem::eulerAngles(gem::fromTo(direction, myEntity.GetForward()));

	if (rotTo.y + myEntity.GetRotation().y < myEntity.GetRotation().y - 0.01)
	{
		myEntity.GetPhysicsActor()->SetAngularVelocity({ 0.f, comp.turningSpeed * -1.f, 0.f });
		myEntity.GetPhysicsActor()->SetLinearVelocity({ 0.f });
	}
	else if (rotTo.y + myEntity.GetRotation().y > myEntity.GetRotation().y + 0.01)
	{
		myEntity.GetPhysicsActor()->SetAngularVelocity({ 0.f, comp.turningSpeed, 0.f });
		myEntity.GetPhysicsActor()->SetLinearVelocity({ 0.f });
	}
	else
	{
		myEntity.GetPhysicsActor()->SetAngularVelocity({ 0.f });

		const float distance = gem::distance(myEntity.GetPosition(), targetPos);
		if (distance > comp.shootDistance)
		{
			const gem::vec3 dir = gem::normalize(direction);
			myEntity.GetPhysicsActor()->SetLinearVelocity(gem::vec3{ dir.x, 0.f, dir.z } *comp.speed);
			return;
		}
		else
		{
			myEntity.GetPhysicsActor()->SetLinearVelocity({ 0.f });
		}

		const float timeSinceShot = std::any_cast<float>(blackboard.at("TimeSinceShot"));
		if (timeSinceShot > 1.f / comp.fireRate)
		{
			ShootBullet(direction, 200000.f + comp.speed);
			blackboard["TimeSinceShot"] = 0.f;
		}
		else
		{
			blackboard["TimeSinceShot"] = timeSinceShot + aDeltaTime;
		}
	}
}

void DecisionTreeActor::Search()
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
			SetVelocity(gem::vec3(reflectedDir.x, 0.f, reflectedDir.z));
		}
	}

	if (rightWisker && rightHit.distance < comp.maxSideDistance)
	{
		const gem::vec3 reflectedDir = gem::reflect(directionRight, rightHit.normal);
		SetVelocity(gem::vec3(reflectedDir.x, 0.f, reflectedDir.z));
	}
	if (leftWisker && leftHit.distance < comp.maxSideDistance)
	{
		const gem::vec3 reflectedDir = gem::reflect(directionLeft, leftHit.normal);
		SetVelocity(gem::vec3(reflectedDir.x, 0.f, reflectedDir.z));
	}

	Volt::Renderer::SubmitLine(myEntity.GetPosition() , myEntity.GetPosition() + currentDirection * 1000  + gem::vec3{ 0.f, 100.f, 0.f });
	Volt::Renderer::SubmitLine(myEntity.GetPosition() + gem::vec3{ 0.f, 100.f, 0.f }, myEntity.GetPosition() + directionRight * comp.maxSideDistance + gem::vec3{ 0.f, 100.f, 0.f });
	Volt::Renderer::SubmitLine(myEntity.GetPosition() + gem::vec3{ 0.f, 100.f, 0.f }, myEntity.GetPosition() + directionLeft * comp.maxSideDistance + gem::vec3{ 0.f, 100.f, 0.f });

}

void DecisionTreeActor::GoToHealingPool()
{
	const gem::vec3 closestHealthWell = FindClosestHealthWell();
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
		SetVelocity(gem::vec3(currentDirection.x,0.f,currentDirection.z));
	}

	if (hitWall && Volt::Physics::GetScene()->Raycast(myEntity.GetPosition(), currentDirection, 10000.f, &forwardHit, { 0 }))
	{
		if (forwardHit.distance < 1000.f)
		{
			const gem::vec3 reflectedDir = gem::reflect(currentDirection, forwardHit.normal);
			SetVelocity(gem::vec3(reflectedDir.x, 0.f, reflectedDir.z));
		}
	}
}

void DecisionTreeActor::KeepHealing()
{
	myEntity.GetPhysicsActor()->SetAngularVelocity({ 0.f, 0.f, 0.f });
	myEntity.GetPhysicsActor()->SetLinearVelocity({ 0.f });
}

void DecisionTreeActor::OnTriggerEnter(Volt::Entity entity, bool isTrigger)
{
	if (entity.HasComponent<HealthWellComponent>())
	{
		myIsOnHealthWell = true;
	}
}

void DecisionTreeActor::OnTriggerExit(Volt::Entity entity, bool isTrigger)
{
	if (entity.HasComponent<HealthWellComponent>())
	{
		myIsOnHealthWell = false;
	}
}

void DecisionTreeActor::ShootBullet(const gem::vec3& direction, const float speed)
{
	auto entity = myEntity.GetScene()->CreateEntity();
	entity.SetPosition(myEntity.GetPosition());

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

		entity.SetPosition(myEntity.GetPosition() + direction * 300.f);
	}

	entity.AddScript("BulletScript");
	entity.GetPhysicsActor()->AddForce(direction * speed, Volt::ForceMode::Force);
}

void DecisionTreeActor::SetVelocity(const gem::vec3& direction)
{
	const auto& comp = myEntity.GetComponent<AIU5DecisionActorComponent>();
	myEntity.GetPhysicsActor()->SetLinearVelocity(direction * comp.speed);

	const gem::vec3 rot = gem::eulerAngles(gem::quatLookAt(gem::vec3{ direction.x, 0.f, direction.z }, { 0.f, 1.f, 0.f }));
	myEntity.SetRotation(rot);
}

const gem::vec3 DecisionTreeActor::FindClosestHealthWell()
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

	return closestWell.GetPosition();
}