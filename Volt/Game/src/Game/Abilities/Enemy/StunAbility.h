#pragma once
#include "Game/Abilities/AbilityBase.h"

class StunAbility : public AbilityBase
{
public:
	StunAbility(const Volt::Entity& aEntity) : AbilityBase(aEntity) {}
	void Cast() override;
};