#pragma once
#include "BossPhase1SB.h"
#include "Game/State machine/StateMachineBase.hpp"

// Phase 1 State Machine
class BossPhase1SM : public StateMachineBase<eBossPhase1State>
{
public:
	BossPhase1SM(const Volt::Entity& aEntity);
	void CreateStates() override;
};