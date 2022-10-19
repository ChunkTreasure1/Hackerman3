#pragma once
#include "Game/State machine/StateBase.hpp"
#include "Game/Enemy/Boss/Phase1/BossPhase1SB.h"
#include "Game/Abilities/Enemy/KnockbackAbility.h"

class BossPhase1KnockbackState : public StateBase<eBossPhase1State>
{
public:
	BossPhase1KnockbackState(const Volt::Entity& aEntity);
	void OnExit() override;
	void OnEnter() override;
	void OnReset() override;
	void OnUpdate(const float& deltaTime) override;
	void OnFixedUpdate() override;
private:

};
