#pragma once
#include "Game/Enemy/Ranged/RangedEnemySB.h"


class RangedEnemyFleeState : public StateBase<eRangedEnemyState>
{
public:
	RangedEnemyFleeState(const Volt::Entity& aEntity);
	void OnExit() override;
	void OnEnter() override;
	void OnReset() override;
	void OnUpdate(const float& deltaTime) override;
	void OnFixedUpdate() override;
private:
	float myMoveSpeed;
};