#pragma once
#include "Game/State machine/StateBase.hpp"
enum class ePlayerState : int32_t
{
	IDLE = 0,
	WALK,
	DIE,
	PRIMARY,
	ARROW,
	SWIPE,
	DASH,
	FEAR,
	BLOODLUST
};