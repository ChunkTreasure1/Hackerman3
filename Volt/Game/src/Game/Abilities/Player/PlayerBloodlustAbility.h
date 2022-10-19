#pragma once
#include "../AbilityBase.h"
class PlayerBloodlustAbility : public AbilityBase
{
public:
	PlayerBloodlustAbility(const Volt::Entity& aEntity) : AbilityBase(aEntity) {}
	void Cast() override;
	void SetUnlocked(bool aIsUnlocked) { myIsUnlocked = aIsUnlocked; };
	bool isUnlocked() { return myIsUnlocked; }

private:
	bool myIsUnlocked = false;

};