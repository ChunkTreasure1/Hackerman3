#pragma once
#include "Game/Abilities/AbilityBase.h"

class GasOffensiveAbility : public AbilityBase
{
public:
	GasOffensiveAbility(const Volt::Entity& aEntity) : AbilityBase(aEntity) {}
	void Cast() override;
	void CastAt(gem::vec3 aPos) override;
};