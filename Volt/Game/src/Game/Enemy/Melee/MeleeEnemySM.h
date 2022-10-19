#pragma once
#include "Game/State machine/StateMachineBase.hpp"
#include "MeleeEnemySB.h"

class MeleeEnemySM : public StateMachineBase<eMeleeEnemyState>
{
public: 
	MeleeEnemySM(const Volt::Entity& aEntity);
	void CreateStates() override;
};;


