#pragma once
#include "Game/State machine/StateBase.hpp"
#include "Game/Enemy/Boss/BossSB.h"
#include "BossPhase1SM.h"

class BossPhase1State : public StateBase<eBossPhaseState>
{
public:
	BossPhase1State(const Volt::Entity& aEntity);
	void OnExit() override;
	void OnEnter() override;
	void OnReset() override;
	void OnUpdate(const float& deltaTime) override;
	void OnFixedUpdate() override;
private:
	Ref<BossPhase1SM> mySM;
};
