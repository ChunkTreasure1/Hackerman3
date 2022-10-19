#pragma once
#include "BossPhase3SB.h"
#include "Game/State machine/StateMachineBase.hpp"

// Phase 3 State Machine
class BossPhase3SM : public StateMachineBase<eBossPhase3State>
{
public:
	BossPhase3SM(const Volt::Entity& aEntity);
	void CreateStates() override;
};