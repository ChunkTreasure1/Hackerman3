#pragma once
#include "Game/Enemy/Bruiser/BruiserEnemySB.h"


class BruiserEnemyWalkState : public StateBase<eBruiserEnemyState>
{
public:
	BruiserEnemyWalkState(const Volt::Entity& aEntity);
	void OnExit() override;
	void OnEnter() override;
	void OnReset() override;
	void OnUpdate(const float& deltaTime) override;
	void OnFixedUpdate() override;
private:
	float myMoveSpeed;
};