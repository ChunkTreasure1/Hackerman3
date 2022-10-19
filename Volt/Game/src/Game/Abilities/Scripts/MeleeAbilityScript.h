#pragma once
#include <Volt/Scripting/ScriptBase.h>

class MeleeAbilityScript : public Volt::ScriptBase
{
public:
	MeleeAbilityScript(const Volt::Entity& aEntity);
	void OnAwake()  override;
	void OnStart() override;
	void OnUpdate(float aDeltaTime)  override;
	void OnStop()  override;
	void OnTriggerEnter(Volt::Entity entity, bool isTrigger) override;
	
	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<MeleeAbilityScript>(aEntity); }
	static WireGUID GetStaticGUID() { return "{15384C4E-5084-45DE-A2F2-0E558CD09319}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }
private:
	float myTimer = 0;
	float myTheTimeMyEnitityHaveUntilAnInevitablyPainfulDemise = 0;
	float myRange = 0;
	float myWidth = 0;
	gem::vec3 myDirecton = {0};
};