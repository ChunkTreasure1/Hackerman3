#pragma once
#include "Game/Abilities/AbilityBase.h"

class ChampionAbility : public AbilityBase
{
public:
	ChampionAbility(const Volt::Entity& aEntity) : AbilityBase(aEntity) {}
	void Cast() override;
};