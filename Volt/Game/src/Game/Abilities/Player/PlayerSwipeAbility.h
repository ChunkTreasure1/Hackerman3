#pragma once
#include "../AbilityBase.h"
class PlayerSwipeAbility : public AbilityBase
{
public:
	PlayerSwipeAbility(const Volt::Entity& aEntity) : AbilityBase(aEntity) {}
	void SetDir(gem::vec3 aPosition);
	void Cast() override; 

	//To do: Do UI stuff to show ability unlocked or not!
	void SetUnlocked(bool aIsUnlocked) { myIsUnlocked = aIsUnlocked; };
	bool isUnlocked() { return myIsUnlocked; }

private:
	gem::vec3 myAttackDir;
	bool myIsUnlocked = false;
};