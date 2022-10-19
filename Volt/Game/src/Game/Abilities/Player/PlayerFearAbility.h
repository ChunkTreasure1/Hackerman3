#pragma once
#include "../AbilityBase.h"
class PlayerFearAbility : public AbilityBase
{
public:
	PlayerFearAbility(const Volt::Entity& aEntity) : AbilityBase(aEntity) {}
	void Cast() override;
	void SetUnlocked(bool aIsUnlocked) { myIsUnlocked = aIsUnlocked; };
	bool isUnlocked() { return myIsUnlocked; }
private:
	bool myIsUnlocked = false;

};