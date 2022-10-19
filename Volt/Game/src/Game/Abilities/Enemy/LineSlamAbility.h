#pragma once
#include "Game/Abilities/AbilityBase.h"

class LineSlamAbility : public AbilityBase
{
public:
	LineSlamAbility(const Volt::Entity& aEntity) : AbilityBase(aEntity) {}
	void Cast() override;
};