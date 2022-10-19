#pragma once
#include "Game/Enemy/Ranged/RangedEnemySB.h"


class RangedEnemyIdleState : public StateBase<eRangedEnemyState>
{
public:
	RangedEnemyIdleState(const Volt::Entity& aEntity);
	void OnExit() override;
	void OnEnter() override;
	void OnReset() override;
	void OnUpdate(const float& deltaTime) override;
	void OnFixedUpdate() override;
private:
};