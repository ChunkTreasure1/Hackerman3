#pragma once

#include "Core.h"

#include "Game/Abilities/AbilityDescriptions.hpp"

class VT_EXPORT Game
{
public:
	void OnStart();
	void OnStop();
	void OnUpdate();

private:
	AbilityData myAbilityData;

};