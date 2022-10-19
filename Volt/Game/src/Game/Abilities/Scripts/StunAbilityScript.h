#pragma once
#include <Volt/Scripting/ScriptBase.h>

class StunAbilityScript : public Volt::ScriptBase
{
public:
	StunAbilityScript(const Volt::Entity& aEntity);
	void OnAwake()  override;
	void OnStart() override;
	void OnUpdate(float aDeltaTime)  override;
	void OnStop()  override;
	void OnTriggerEnter(Volt::Entity, bool isTrigger) override;

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<StunAbilityScript>(aEntity); }
	static WireGUID GetStaticGUID() { return "{A0670F2C-7983-4425-B982-9B1AE2232B5B}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }
private:
	float myTimer = 0;
	float myTheTimeMyEnitityHaveUntilAnInevitablyPainfulDemise = 0;
	float myRange = 0;
	float myWidth = 0;

};