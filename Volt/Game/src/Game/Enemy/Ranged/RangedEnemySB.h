#pragma once
#include "Game/State machine/StateBase.hpp"

enum class eRangedEnemyState
{
	IDLE,
	WALK,
	ATTACK,
	FLEE,
};

//class MeleeEnemySB : public StateBase<eMeleeEnemyState>
//{
//
//};