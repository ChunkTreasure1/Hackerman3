#pragma once
#include "Game/State machine/StateBase.hpp"
#include "Game/Enemy/Boss/Phase3/BossPhase3SB.h"
#include "Game/Abilities/Enemy/KnockbackAbility.h"

class BossPhase3KnockbackState : public StateBase<eBossPhase3State>
{
public:
	BossPhase3KnockbackState(const Volt::Entity& aEntity);
	void OnExit() override;
	void OnEnter() override;
	void OnReset() override;
	void OnUpdate(const float& deltaTime) override;
	void OnFixedUpdate() override;
private:

};
