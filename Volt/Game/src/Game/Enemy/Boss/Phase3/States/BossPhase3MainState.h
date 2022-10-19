#pragma once
#include "Game/Enemy/Boss/Phase3/BossPhase3SB.h"
#include "Game/State machine/StateBase.hpp"

class BossPhase3MainState : public StateBase<eBossPhase3State>
{
public:
	BossPhase3MainState(const Volt::Entity& aEntity);
	void OnExit() override;
	void OnEnter() override;
	void OnReset() override;
	void OnUpdate(const float& deltaTime) override;
	void OnFixedUpdate() override;

private:
	Volt::Entity myKnockbackEntity;
	float myKnockbackTimer = 0;
};