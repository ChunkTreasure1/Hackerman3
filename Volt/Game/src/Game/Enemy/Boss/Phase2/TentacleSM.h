#pragma once
#include "Game/State machine/StateMachineBase.hpp"
#include "Game/Enemy/Boss/Phase2/TentacleSB.h"

class TentacleSM : public StateMachineBase<eTentacleState>
{
public:
	TentacleSM(const Volt::Entity& aEntity);
	void CreateStates() override;
};