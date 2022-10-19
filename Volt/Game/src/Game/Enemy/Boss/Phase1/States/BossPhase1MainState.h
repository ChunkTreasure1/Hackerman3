#pragma once
#include "Game/Enemy/Boss/Phase1/BossPhase1SB.h"
#include "Game/State machine/StateBase.hpp"

class BossPhase1MainState : public StateBase<eBossPhase1State>
{
public:
	BossPhase1MainState(const Volt::Entity& aEntity);
	void OnExit() override;
	void OnEnter() override;
	void OnReset() override;
	void OnUpdate(const float& deltaTime) override;
	void OnFixedUpdate() override;

private:
	Volt::Entity myKnockbackEntity;
	float myKnockbackTimer = 0;
};