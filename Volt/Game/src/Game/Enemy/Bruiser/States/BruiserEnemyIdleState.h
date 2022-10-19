#pragma once
#include "Game/Enemy/Bruiser/BruiserEnemySB.h"


class BruiserEnemyIdleState : public StateBase<eBruiserEnemyState>
{
public:
	BruiserEnemyIdleState(const Volt::Entity& aEntity);
	void OnExit() override;
	void OnEnter() override;
	void OnReset() override;
	void OnUpdate(const float& deltaTime) override;
	void OnFixedUpdate() override;
private:
};