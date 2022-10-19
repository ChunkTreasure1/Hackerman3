#pragma once
#include "Game/State machine/StateBase.hpp"
#include "Game/Enemy/Boss/BossSB.h"
#include "BossPhase3SM.h"

class BossPhase3State : public StateBase<eBossPhaseState>
{
public:
	BossPhase3State(const Volt::Entity& aEntity);
	void OnExit() override;
	void OnEnter() override;
	void OnReset() override;
	void OnUpdate(const float& deltaTime) override;
	void OnFixedUpdate() override;
private:
	Ref<BossPhase3SM> mySM;
};
