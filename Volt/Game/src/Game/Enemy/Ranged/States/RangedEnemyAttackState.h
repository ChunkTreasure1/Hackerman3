#pragma once
#include "Game/Enemy/Ranged/RangedEnemySB.h"
#include "Game/Abilities/Enemy/RangedAbility.h"

class RangedEnemyAttackState : public StateBase<eRangedEnemyState>
{
public:
	RangedEnemyAttackState(const Volt::Entity& aEntity);
	void OnExit() override;
	void OnEnter() override;
	void OnReset() override;
	void OnUpdate(const float& deltaTime) override;
	void OnFixedUpdate() override;
private:
	Ref<RangedAbility> myRangedAbility;
	float myFleeDistance;
};