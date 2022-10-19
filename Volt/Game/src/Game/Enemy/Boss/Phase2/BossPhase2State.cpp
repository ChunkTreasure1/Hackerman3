#include "BossPhase2State.h"
#include <Volt/Components/Components.h>
#include <Volt/Components/PhysicsComponents.h>
#include "GEM/vector/type_vector2.h"
#include "Game/Enemy/Boss/Phase2/TentacleScript.h"
#include "Game/Enemy/Boss/BossScript.h"
#include <Volt/Log/Log.h>
#include "Game/Abilities/Enemy/GasShieldAbility.h"
#include <Volt/Animation/AnimationManager.h>

BossPhase2State::BossPhase2State(const Volt::Entity& aEntity) : StateBase(aEntity)
{}

void BossPhase2State::OnExit()
{
	myShield->Cancel();
	for (auto e : myTentacles)
		e.GetScene()->RemoveEntity(e);
}

void BossPhase2State::OnEnter()
{
	VT_CORE_INFO("Boss Phase 2 entered");
	const auto& bossComp = myEntity.GetComponent<Volt::BossComponent>();
	const auto& bossHealthComp = myEntity.GetComponent<Volt::HealthComponent>();

	float phaseHealthPercent = bossComp.phase2p - bossComp.phase3p;
	int tentacleHealt = 0.25f * (bossHealthComp.maxHealth * phaseHealthPercent);

	float tentacleSpawnDistance = myEntity.GetComponent<Volt::BossComponent>().tentacleSpawnDistance;

	{
		auto ent = myEntity.GetScene()->CreateEntity();
		myTentacles.push_back(ent);

		gem::vec2 dir(-1, 1);
		dir = gem::normalize(dir);
		dir *= tentacleSpawnDistance;
		gem::vec3 bossPos = myEntity.GetWorldPosition();

		auto& ent_hp = ent.AddComponent<Volt::HealthComponent>();
		ent_hp.health = tentacleHealt;
		ent_hp.maxHealth = ent_hp.health;

		ent.SetPosition({ bossPos.x + dir.x, bossPos.y , bossPos.z + dir.y });
		ent.AddScript("TentacleScript");

		ent.AddComponent<Volt::CapsuleColliderComponent>();
		Volt::RigidbodyComponent rigid{};
		rigid.bodyType = Volt::BodyType::Dynamic;
		rigid.layerId = 3;
		rigid.isKinematic = true;
		ent.AddComponent<Volt::RigidbodyComponent>(&rigid);
	}

	{
		auto ent = myEntity.GetScene()->CreateEntity();
		myTentacles.push_back(ent);

		gem::vec2 dir(1, 1);
		dir = gem::normalize(dir);
		dir *= tentacleSpawnDistance;
		gem::vec3 bossPos = myEntity.GetWorldPosition();

		auto& ent_hp = ent.AddComponent<Volt::HealthComponent>();
		ent_hp.health = tentacleHealt;
		ent_hp.maxHealth = ent_hp.health;

		ent.SetPosition({ bossPos.x + dir.x, bossPos.y , bossPos.z + dir.y });
		ent.AddScript("TentacleScript");

		ent.AddComponent<Volt::CapsuleColliderComponent>();
		Volt::RigidbodyComponent rigid{};
		rigid.bodyType = Volt::BodyType::Dynamic;
		rigid.layerId = 3;
		rigid.isKinematic = true;
		ent.AddComponent<Volt::RigidbodyComponent>(&rigid);
	}

	{
		auto ent = myEntity.GetScene()->CreateEntity();
		myTentacles.push_back(ent);

		gem::vec2 dir(1, -1);
		dir = gem::normalize(dir);
		dir *= tentacleSpawnDistance;
		gem::vec3 bossPos = myEntity.GetWorldPosition();

		auto& ent_hp = ent.AddComponent<Volt::HealthComponent>();
		ent_hp.health = tentacleHealt;
		ent_hp.maxHealth = ent_hp.health;

		ent.SetPosition({ bossPos.x + dir.x, bossPos.y , bossPos.z + dir.y });
		ent.AddScript("TentacleScript");

		ent.AddComponent<Volt::CapsuleColliderComponent>();
		Volt::RigidbodyComponent rigid{};
		rigid.bodyType = Volt::BodyType::Dynamic;
		rigid.layerId = 3;
		rigid.isKinematic = true;
		ent.AddComponent<Volt::RigidbodyComponent>(&rigid);
	}

	{
		auto ent = myEntity.GetScene()->CreateEntity();
		myTentacles.push_back(ent);

		gem::vec2 dir(-1, -1);
		dir = gem::normalize(dir);
		dir *= tentacleSpawnDistance;
		gem::vec3 bossPos = myEntity.GetWorldPosition();

		auto& ent_hp = ent.AddComponent<Volt::HealthComponent>();
		ent_hp.health = tentacleHealt;
		ent_hp.maxHealth = ent_hp.health;

		ent.SetPosition({ bossPos.x + dir.x, bossPos.y , bossPos.z + dir.y });
		ent.AddScript("TentacleScript");

		ent.AddComponent<Volt::CapsuleColliderComponent>();
		Volt::RigidbodyComponent rigid{};
		rigid.bodyType = Volt::BodyType::Dynamic;
		rigid.layerId = 3;
		rigid.isKinematic = true;
		ent.AddComponent<Volt::RigidbodyComponent>(&rigid);
	}

	myShield = CreateRef<GasShieldAbility>(myEntity);
	bool eys = myShield->IsCastable();
	myShield->Cast();

	myEntity.GetComponent<Volt::AnimatedCharacterComponent>().currentAnimation = 1;
	myEntity.GetComponent<Volt::AnimatedCharacterComponent>().currentStartTime = Volt::AnimationManager::globalClock;

}

void BossPhase2State::OnReset()
{}

void BossPhase2State::OnUpdate(const float& deltaTime)
{

	myShield->Update(deltaTime);
	int phaseHealth = 0;
	for (int i = myTentacles.size() - 1; i >= 0; i--)
	{
		float tentHealth = myTentacles[i].GetComponent<Volt::HealthComponent>().health;
		phaseHealth += tentHealth;
		if (myTentacles[i].GetComponent<Volt::HealthComponent>().health < 0)
		{
			myTentacles[i].GetScene()->RemoveEntity(myTentacles[i]);
			myTentacles.erase(myTentacles.begin() + i);
		}
	}
	auto& bossHealthComponent = myEntity.GetComponent<Volt::HealthComponent>();
	auto& bossComponent = myEntity.GetComponent<Volt::BossComponent>();
	bossHealthComponent.health = (bossHealthComponent.maxHealth * bossComponent.phase3p) + phaseHealth;

	// BAD
	// CHANGE TO COMP MAXHEALTH
	float startHP = myEntity.GetScript<BossScript>("BossScript")->GetStartHP();
	float currentHP = myEntity.GetComponent<Volt::HealthComponent>().health;
	float res = currentHP / startHP;

	if (res <= myEntity.GetComponent<Volt::BossComponent>().phase3p && myTentacles.empty())
		SetTransition(eBossPhaseState::PHASE3);

	// ENDBAD
}

void BossPhase2State::OnFixedUpdate()
{}
