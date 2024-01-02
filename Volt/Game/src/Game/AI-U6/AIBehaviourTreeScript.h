#pragma once
#include "Volt/Scene/Entity.h"
#include "Volt/Scripting/ScriptBase.h"
#include "Volt/Scripting/ScriptRegistry.h"
#include "Volt/Log/Log.h"
#include "Volt/Components/Components.h"
#include "AIBT.h"
#include "../AI-U5/PollingStationU5.h"
#include <any>

class AIBehaviourTreeScript : public Volt::ScriptBase
{
public:
	AIBehaviourTreeScript(const Volt::Entity& aEntity);

	void OnCollisionEnter(Volt::Entity entity);

	void Wander();
	bool CanSeeEnemy();
	bool IsInRange();
	bool Shoot();
	bool LowHealth();
	bool IsDead();
	bool IsOnHealthWell();
	bool UpdateRespawn();
	void GoToHealthWell();
	Volt::Entity GetClosestHealthWell();

	void OnTriggerEnter(Volt::Entity entity, bool isTrigger)override;
	void OnTriggerExit(Volt::Entity entity, bool isTrigger) override;


	int GetHealth() { return myEntity.GetComponent<AIU5HealthComponent>().currentHealth; }

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<AIBehaviourTreeScript>(aEntity); }
	static WireGUID GetStaticGUID() { return "{D8D2EA19-622F-4DBF-9D7F-C88AE03E1A07}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }

private:
	Volt::Entity myEnemy;
	gem::vec3 myStartPos = 0.f;
	gem::vec3 myStartRot = 0.f;

	float myShootTimer = 0.f;
	float myReloadTimer = 0.f;
	float myNewMoveDirTimer = 0.f;
	float myHealingTimer = 0.f;
	float myDeathTimer = 0;

	float myDT;

	int myHealth = 0.f;
	int myMagSize = 0;

	bool testBool = false;
	bool myIsOnHealthWell = true;

	AIBT myBT;

	void OnStart() override;
	void OnUpdate(float aDeltaTime) override;

	std::unordered_map<std::string, std::any> blackboard;
};
