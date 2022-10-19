#pragma once
#include "Game/State machine/StateMachineBase.hpp"
#include "BruiserEnemySB.h"

class BruiserEnemySM : public StateMachineBase<eBruiserEnemyState>
{
public: 
	BruiserEnemySM(const Volt::Entity& aEntity);
	void CreateStates() override;
};;


