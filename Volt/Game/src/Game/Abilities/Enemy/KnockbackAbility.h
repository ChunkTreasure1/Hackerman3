#pragma once
#include "Game/Abilities/AbilityBase.h"

class KnockbackAbility : public AbilityBase
{
public:
	KnockbackAbility(const Volt::Entity& aEntity) : AbilityBase(aEntity) {}
	void Cast() override;
};