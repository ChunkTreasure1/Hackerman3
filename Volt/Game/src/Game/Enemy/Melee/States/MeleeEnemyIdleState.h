#pragma once
#include "Game/Enemy/Melee/MeleeEnemySB.h"


class MeleeEnemyIdleState : public StateBase<eMeleeEnemyState>
{
public:
	MeleeEnemyIdleState(const Volt::Entity& aEntity);
	void OnExit() override;
	void OnEnter() override;
	void OnReset() override;
	void OnUpdate(const float& deltaTime) override;
	void OnFixedUpdate() override;
private:
};