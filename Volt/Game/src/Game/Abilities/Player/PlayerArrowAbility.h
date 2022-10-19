#pragma once
#include "../AbilityBase.h"
class PlayerArrowAbility : public AbilityBase
{
public:
	PlayerArrowAbility(const Volt::Entity& aEntity) : AbilityBase(aEntity){}
	void SetDir(gem::vec3 aPosition);
	void Cast() override;

private:
	gem::vec3 myShootDirection;
};