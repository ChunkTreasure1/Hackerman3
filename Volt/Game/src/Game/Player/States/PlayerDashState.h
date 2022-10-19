#pragma once
#include "Game/Player/PlayerSB.h"

class PlayerDashState : public StateBase<ePlayerState>
{
public:
	PlayerDashState(const Volt::Entity& aEntity);
	void OnEnter() override;
	void OnUpdate(const float& deltaTime) override;
	void OnExit() override;
	void OnReset() override;
	void OnFixedUpdate() override;

private:
	float myTimeActive = 0;

};