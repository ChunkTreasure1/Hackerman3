#pragma once
#include "Game/Player/PlayerSB.h"

class PlayerDeathState : public StateBase<ePlayerState>
{
public:
	PlayerDeathState(const Volt::Entity& aEntity);
	void OnEnter() override;
	void OnUpdate(const float& deltaTime) override;
	void OnExit() override;
	void OnReset() override;
	void OnFixedUpdate() override;

private:

};