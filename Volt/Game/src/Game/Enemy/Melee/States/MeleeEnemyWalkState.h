#pragma once
#include "Game/Enemy/Melee/MeleeEnemySB.h"


class MeleeEnemyWalkState : public StateBase<eMeleeEnemyState>
{
public:
	MeleeEnemyWalkState(const Volt::Entity& aEntity);
	void OnExit() override;
	void OnEnter() override;
	void OnReset() override;
	void OnUpdate(const float& deltaTime) override;
	void OnFixedUpdate() override;
private:
	float myRangeP;
	float myMoveSpeed;
};