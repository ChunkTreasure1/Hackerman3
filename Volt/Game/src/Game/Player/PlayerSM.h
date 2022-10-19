#pragma once
#include "Game/State machine/StateMachineBase.hpp"
#include "PlayerSB.h"
#include "States/PlayerIdleState.h"
#include "States/PlayerWalkState.h"

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