#pragma once
#include "../AbilityBase.h"
class PlayerDashAbility : public AbilityBase
{
public:
	PlayerDashAbility(const Volt::Entity& aEntity) : AbilityBase(aEntity) {}
	void SetDir(gem::vec3 aPosition);

	void Cast() override;
	void SetUnlocked(bool aIsUnlocked) { myIsUnlocked = aIsUnlocked; };
	bool isUnlocked() { return myIsUnlocked; }

private:
	gem::vec3 myDashDirection;
	bool myIsUnlocked = false;

};