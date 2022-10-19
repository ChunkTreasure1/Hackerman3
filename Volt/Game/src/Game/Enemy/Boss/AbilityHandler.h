#pragma once
#include "Game/Abilities/AbilityDescriptions.hpp"
#include "Game/Abilities/AbilityBase.h"
#include <Volt/Scene/Entity.h>

#include <unordered_map>


// BossSM check if cd ready, if so change state, play anim and when in anim and ready cast. 



enum class eBossAbility
{
	LINESLAM,
	KNOCKBACK,
	GASATTACK,
	GASSHIELD,
	//TENTACLESPAWN
};

class AbilityHandler
{
public:
	AbilityHandler(const Volt::Entity& aEntity);
	~AbilityHandler();

	bool IsCastable(const eBossAbility& aAbility);
	bool Cast(const eBossAbility& aAbility);
	bool CastAt(const eBossAbility& aAbility, const gem::vec3& aPos);
	bool Cancel(const eBossAbility& aAbility);
	void Update(const float& deltaTime);

	Volt::Entity GetEntity() { return myEntity; }

private:
	const Volt::Entity myEntity;
	std::unordered_map<eBossAbility, Ref<AbilityBase>> myAbilities;
};