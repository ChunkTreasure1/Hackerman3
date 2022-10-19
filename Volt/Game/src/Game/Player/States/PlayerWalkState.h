#pragma once
#include "Game/Player/PlayerSB.h"

class PlayerWalkState : public StateBase<ePlayerState>
{
public:
	PlayerWalkState(const Volt::Entity& aEntity);
	void OnEnter() override;
	void OnUpdate(const float& deltaTime) override;
	void OnExit() override;
	void OnReset() override;
	void OnFixedUpdate() override;

	void MoveTowardsTarget(const float& deltaTime);

	void SetTargetPos(gem::vec3 aPos);
	void SetViewportValues(gem::vec2 aVPMousePos, gem::vec2 aVPSize);

private:
	bool myMovingToTarget;

	gem::vec3 myTargetPosition;
	gem::vec2 myViewportMousePos;
	gem::vec2 myViewportSize;
};