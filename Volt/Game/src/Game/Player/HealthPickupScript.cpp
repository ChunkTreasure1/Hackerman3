#include "HealthPickupScript.h"
#include "Volt/Components/PhysicsComponents.h"
#include "Volt/Physics/Physics.h"
#include "Volt/Physics/PhysicsScene.h"
#include "GEM/gem.h"
#include "Volt/Utility/Random.h"
#include <random>
VT_REGISTER_SCRIPT(HealthPickupScript)

HealthPickupScript::HealthPickupScript(const Volt::Entity& aEntity)
	:ScriptBase(aEntity)
{
}

void HealthPickupScript::OnAwake()
{
	Volt::Entity tempEnt;
	myEntity.GetScene()->GetRegistry().ForEach<Volt::PlayerComponent>([&](Wire::EntityId id, const Volt::PlayerComponent& scriptComp)
		{
			tempEnt = { id, myEntity.GetScene() };
		});

	if(tempEnt) myPlayerTarget = tempEnt;

	myEntity.SetScale({ 0.2f,0.2f,0.2f });
	Volt::RigidbodyComponent rigidInit{};
	rigidInit.bodyType = Volt::BodyType::Dynamic;
	rigidInit.disableGravity = true;
	auto& rigidComp = myEntity.AddComponent<Volt::RigidbodyComponent>(&rigidInit);

	gem::vec3 randomDir;

	randomDir = { Volt::Random::Float(-50, 50), Volt::Random::Float(0, 50), Volt::Random::Float(-50, 50)};
	gem::vec3 randomForceVec = gem::normalize(randomDir)  * 200.0f;
	randomForceVec.y = gem::abs(randomForceVec.y);
	myEntity.GetPhysicsActor()->AddForce(randomForceVec, Volt::ForceMode::Impulse);
}

void HealthPickupScript::OnUpdate(float aDeltaTime)
{
	if (myPlayerTarget)
	{
		float dist = gem::length(myPlayerTarget.GetPosition() - myEntity.GetPosition());
		if (dist < 200.0f)
		{
			if (myExitInterpolation < 1) 
			{
				myExitInterpolation += 0.15f * aDeltaTime;
				myEntity.SetPosition(gem::lerp(myEntity.GetPosition(), myPlayerTarget.GetPosition(), myExitInterpolation));
			}
		}
		else { myExitInterpolation = 0.f; }

		if (dist < 80.0f)
		{
			myPlayerTarget.GetComponent<Volt::HealthComponent>().health += 1;
			myEntity.GetScene()->RemoveEntity(myEntity);
		}
	}
}

void HealthPickupScript::OnFixedUpdate(float aDeltaTime)
{
	if (gem::length(myEntity.GetPhysicsActor()->GetLinearVelocity())> 0)
	{
		if (myInitInterpolation < 1)
		{
			myInitInterpolation += 0.05f * aDeltaTime;
			gem::vec3 zeroVec{ 0 };
			myEntity.GetPhysicsActor()->SetLinearVelocity(gem::lerp(myEntity.GetPhysicsActor()->GetLinearVelocity(), zeroVec, myInitInterpolation));
		}
		else { myInitInterpolation = 1.f; }

	}
}
