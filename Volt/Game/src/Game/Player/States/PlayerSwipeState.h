#pragma once
#include "Game/Player/PlayerSB.h"

class PlayerSwipeState : public StateBase<ePlayerState>
{
public:
	PlayerSwipeState(const Volt::Entity& aEntity);
	void OnEnter() override;
	void OnUpdate(const float& deltaTime) override;
	void OnExit() override;
	void OnReset() override;
	void OnFixedUpdate() override;

	void PlayerIsMoving(bool isMoving);

private:
	bool myIsMoving = false;
	float myTimeActive = 0;
};