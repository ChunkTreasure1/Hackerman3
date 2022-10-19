#pragma once
#include "Game/Enemy/Boss/Phase2/TentacleSB.h"
#include "Game/State machine/StateBase.hpp"

class TentacleAttackState : public StateBase<eTentacleState>
{
public:
	TentacleAttackState(const Volt::Entity& aEntity);
	void OnExit() override;
	void OnEnter() override;
	void OnReset() override;
	void OnUpdate(const float& deltaTime) override;
	void OnFixedUpdate() override;

private:
	bool myHasCast = false;
	float myTimer = 0; 
};