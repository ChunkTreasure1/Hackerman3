#pragma once
#include "Game/State machine/StateMachineBase.hpp"
#include "RangedEnemySB.h"

class RangedEnemySM : public StateMachineBase<eRangedEnemyState>
{
public: 
	RangedEnemySM(const Volt::Entity& aEntity);
	void CreateStates() override;
};;


