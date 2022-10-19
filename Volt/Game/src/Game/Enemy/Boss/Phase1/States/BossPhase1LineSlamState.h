#pragma once
#include "Game/State machine/StateBase.hpp"
#include "Game/Enemy/Boss/Phase1/BossPhase1SB.h"

class BossPhase1LineSlamState : public StateBase<eBossPhase1State>
{
public:
	BossPhase1LineSlamState(const Volt::Entity& aEntity);
	void OnExit() override;
	void OnEnter() override;
	void OnReset() override;
	void OnUpdate(const float& deltaTime) override;
	void OnFixedUpdate() override;
private:
	float myAnimationTime = 0;
};
