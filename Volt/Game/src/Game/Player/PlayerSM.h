#pragma once
#include "Game/State machine/StateMachineBase.hpp"
#include "PlayerSB.h"
#include "States/PlayerIdleState.h"
#include "States/PlayerWalkState.h"
#include "States/PlayerPrimaryState.h"
#include "States/PlayerArrowState.h"
#include "States/PlayerSwipeState.h"
#include "States/PlayerDashState.h"
#include "States/PlayerFearState.h"
#include "States/PlayerBloodLustState.h"
#include "States/PlayerDeathState.h"

class PlayerSM : public StateMachineBase<ePlayerState>
{
public:
	PlayerSM(const Volt::Entity& aEntity);
	void CreateStates() override;

	template<typename T>
	Ref<T> GetPlayerState(ePlayerState aState);
};

template<typename T>
inline Ref<T> PlayerSM::GetPlayerState(ePlayerState aState)
{
	if (myStates[aState])
	{
		return std::reinterpret_pointer_cast<T>(myStates[aState]);
	}
	else
	{
		return nullptr;
	}
}