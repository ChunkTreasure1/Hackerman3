#pragma once
#include "Game/Abilities/AbilityBase.h"

class MeleeAbility : public AbilityBase
{
public:
	MeleeAbility(const Volt::Entity& aEntity) : AbilityBase(aEntity) {}
	void Cast() override;
};