#pragma once
#include <Volt/Scripting/ScriptBase.h>

class RangedAbilityScript : public Volt::ScriptBase
{
public:
	RangedAbilityScript(const Volt::Entity& aEntity);
	void OnAwake()  override;
	void OnStart() override;
	void OnUpdate(float aDeltaTime)  override;
	void OnStop()  override;
	void  OnTriggerEnter(Volt::Entity, bool) override;

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<RangedAbilityScript>(aEntity); }
	static WireGUID GetStaticGUID() { return "{E1FC2EE0-EDFB-4CB1-996A-87DAE80077BC}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }
private:
	float myDistanceTraveled = 0;
};