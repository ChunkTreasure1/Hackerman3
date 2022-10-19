#pragma once
#include "../AbilityBase.h"
class PlayerPrimaryAbility : public AbilityBase
{
public:
	PlayerPrimaryAbility(const Volt::Entity& aEntity) : AbilityBase(aEntity){}
	void SetDir(gem::vec3 aPosition);
	void Cast() override;

private:
	gem::vec3 myAttackDir;
};