#pragma once
#include "Game/Enemy/Boss/BossSB.h"
#include "Game/State machine/StateBase.hpp"
#include "Game/Abilities/AbilityBase.h"

class BossPhase2State : public StateBase<eBossPhaseState>
{
public:
	BossPhase2State(const Volt::Entity& aEntity);
	void OnExit() override;
	void OnEnter() override;
	void OnReset() override;
	void OnUpdate(const float& deltaTime) override;
	void OnFixedUpdate() override;
private:
	std::vector<Volt::Entity> myTentacles;
	Ref<AbilityBase> myShield;
};