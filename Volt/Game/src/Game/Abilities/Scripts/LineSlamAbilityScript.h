#pragma once
#include <Volt/Scripting/ScriptBase.h>

class LineSlamAbilityScript : public Volt::ScriptBase
{
public:
	LineSlamAbilityScript(const Volt::Entity& aEntity);
	void OnAwake() override;
	void OnStart() override;
	void OnUpdate(float aDeltaTime) override;
	void OnStop() override;
	void OnTriggerEnter(Volt::Entity, bool) override;

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<LineSlamAbilityScript>(aEntity); }
	static WireGUID GetStaticGUID() { return "{AE8D1F63-C68F-444E-BFFE-235ACD57741B}"_guid; };
	WireGUID GetGUID()override { return GetStaticGUID(); }
private:
	float myTimer = 0;
	
};