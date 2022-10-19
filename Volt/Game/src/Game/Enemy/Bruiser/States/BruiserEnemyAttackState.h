#pragma once
#include "Game/Enemy/Bruiser/BruiserEnemySB.h"
#include "Game/Abilities/Enemy/StunAbility.h"

class BruiserEnemyAttackState : public StateBase<eBruiserEnemyState>
{
public:
	BruiserEnemyAttackState(const Volt::Entity& aEntity);
	void OnExit() override;
	void OnEnter() override;
	void OnReset() override;
	void OnUpdate(const float& deltaTime) override;
	void OnFixedUpdate() override;
private:
	Ref<StunAbility> myBruiserAbility;
	bool myIsCasting = false;
};