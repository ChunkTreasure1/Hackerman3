#pragma once
#include "Game/Enemy/Melee/MeleeEnemySB.h"
#include "Game/Abilities/Enemy/MeleeAbility.h"

class MeleeEnemyAttackState : public StateBase<eMeleeEnemyState>
{
public:
	MeleeEnemyAttackState(const Volt::Entity& aEntity);
	void OnExit() override;
	void OnEnter() override;
	void OnReset() override;
	void OnUpdate(const float& deltaTime) override;
	void OnFixedUpdate() override;
private:
	Ref<MeleeAbility> myMeleeAbility;
	float myRangeP;
	float myAnimationTime = 0;
	bool myAnimationStarted = false;
};