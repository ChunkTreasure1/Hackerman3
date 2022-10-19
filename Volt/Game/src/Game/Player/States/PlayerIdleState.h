#pragma once
#include "Game/Player/PlayerSB.h"

class PlayerIdleState : public StateBase<ePlayerState>
{
public:
	PlayerIdleState(const Volt::Entity& aEntity);
	void OnEnter() override;
	void OnUpdate(const float& deltaTime) override;
	void OnExit() override;
	void OnReset() override;
	void OnFixedUpdate() override;

private:
};