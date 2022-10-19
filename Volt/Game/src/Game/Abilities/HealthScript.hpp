#pragma once
#include "Volt/Scene/Entity.h"
#include "Volt/Scripting/ScriptBase.h"
#include "Volt/Scripting/ScriptRegistry.h"
#include "Volt/Log/Log.h"

class HealthScript : public Volt::ScriptBase 
{
public:
	HealthScript(const Volt::Entity& aEntity) : ScriptBase(aEntity){}

	void SetHealth(int aHealthAmount) { myHealth = aHealthAmount; }
	void TakeDamage(int aDamageAmount);

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<HealthScript>(aEntity); }
	static WireGUID GetStaticGUID() { return "{855D53FE-1DA8-44A0-B93D-88EDF1A85B59}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }

private:
	void Die();

	int myHealth = 3;
};

VT_REGISTER_SCRIPT(HealthScript)

void HealthScript::TakeDamage(int aDamageAmount)
{
	myHealth -= aDamageAmount;
	if (myHealth <= 0) { Die(); }
}

void HealthScript::Die() 
{
	myEntity.GetScene()->RemoveEntity(myEntity);
}
