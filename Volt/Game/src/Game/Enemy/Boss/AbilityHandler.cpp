#include "AbilityHandler.h"
#include "Game/Abilities/Enemy/LineSlamAbility.h"
#include "Game/Abilities/Enemy/KnockbackAbility.h"
#include "Game/Abilities/Enemy/GasShieldAbility.h"
#include "Game/Abilities/Enemy/GasOffensiveAbility.h"

AbilityHandler::AbilityHandler(const Volt::Entity& aEntity) : myEntity(aEntity)
{
	myAbilities.insert({ eBossAbility::LINESLAM,	CreateRef<LineSlamAbility>(aEntity) });
	myAbilities.insert({ eBossAbility::KNOCKBACK,	CreateRef<KnockbackAbility>(aEntity) });
	myAbilities.insert({ eBossAbility::GASATTACK,	CreateRef<GasOffensiveAbility>(aEntity) });
	myAbilities.insert({ eBossAbility::GASSHIELD,	CreateRef<GasShieldAbility>(aEntity) });
	// SUMMON ABILITY
}

AbilityHandler::~AbilityHandler()
{}

bool AbilityHandler::IsCastable(const eBossAbility& aAbility)
{
	if (myAbilities.find(aAbility) == myAbilities.end())
		return false;
	return myAbilities[aAbility]->IsCastable();
}

bool AbilityHandler::Cast(const eBossAbility& aAbility)
{
	if (myAbilities.find(aAbility) == myAbilities.end())
		return false;
	// Debug purp
	if (!myAbilities[aAbility]->IsCastable())
		return false;
	myAbilities[aAbility]->Cast();

	return true;
}

bool AbilityHandler::CastAt(const eBossAbility& aAbility, const gem::vec3& aPos)
{
	if (myAbilities.find(aAbility) == myAbilities.end())
		return false;
	// Debug purp
	if (!myAbilities[aAbility]->IsCastable())
		return false;
	myAbilities[aAbility]->CastAt(aPos);

	return true;
}

bool AbilityHandler::Cancel(const eBossAbility& aAbility)
{
	return false;
}

void AbilityHandler::Update(const float& deltaTime)
{
	for (auto it = myAbilities.begin(); it != myAbilities.end(); ++it)
		it->second->Update(deltaTime);
}
