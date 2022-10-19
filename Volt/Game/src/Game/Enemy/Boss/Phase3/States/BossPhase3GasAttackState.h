#pragma once
#include "Game/State machine/StateBase.hpp"
#include "Game/Enemy/Boss/Phase3/BossPhase3SB.h"

class BossPhase3GasAttackState : public StateBase<eBossPhase3State>
{
public:
	BossPhase3GasAttackState(const Volt::Entity& aEntity);
	void OnExit() override;
	void OnEnter() override;
	void OnReset() override;
	void OnUpdate(const float& deltaTime) override;
	void OnFixedUpdate() override;
private:
};
