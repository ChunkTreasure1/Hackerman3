#pragma once

#include <Volt/Scripting/ScriptBase.h>

SERIALIZE_COMPONENT((struct HealthWellComponent
{
	PROPERTY(Name = Heal Rate) float healRate = 10.f;


	CREATE_COMPONENT_GUID("{A696D656-8962-4370-8C7E-79217E531BA6}"_guid);
}), HealthWellComponent);

class HealthWellScript : public Volt::ScriptBase
{
public:
	HealthWellScript(Volt::Entity entity);

	void OnUpdate(float aDeltaTime) override;
	void OnTriggerEnter(Volt::Entity entity, bool isTrigger) override;
	void OnTriggerExit(Volt::Entity entity, bool isTrigger) override;

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<HealthWellScript>(aEntity); }
	static WireGUID GetStaticGUID() { return "{3485D4EE-7ABC-489E-AB80-417DF3B20B25}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }

private:
	std::vector<Volt::Entity> myEntities;

	float myTimer = 1.f;
};