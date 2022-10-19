#pragma once
#include "Game/State machine/StateMachineBase.hpp"
#include "BossSB.h"

// Phase State Machine 
class BossSM : public StateMachineBase<eBossPhaseState>
{
public:
	BossSM(const Volt::Entity& aEntity);
	void CreateStates() override;
};

