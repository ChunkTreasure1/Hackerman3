#pragma once
#include "Game/Abilities/AbilityBase.h"

class GasShieldAbility : public AbilityBase
{
public:
	GasShieldAbility(const Volt::Entity& aEntity) : AbilityBase(aEntity) {}
	void Cast() override;
	void Cancel() override;

private:
	Volt::Entity myShieldEntity;
};