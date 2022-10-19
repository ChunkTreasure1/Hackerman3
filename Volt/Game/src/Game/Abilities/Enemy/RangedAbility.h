#pragma once
#include "Game/Abilities/AbilityBase.h"

class RangedAbility : public AbilityBase
{
public:
	RangedAbility(const Volt::Entity& aEntity) : AbilityBase(aEntity) {}
	void Cast() override;
};