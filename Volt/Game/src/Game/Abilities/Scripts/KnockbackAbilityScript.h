#pragma once
#include <Volt/Scripting/ScriptBase.h>

// Needs implementations
class KncokbackAbilityScript : public Volt::ScriptBase
{
public:
	KncokbackAbilityScript(const Volt::Entity& aEntity);
	void OnAwake()  override;
	void OnStart() override;
	void OnUpdate(float aDeltaTime)  override;
	void OnStop()  override;

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<KncokbackAbilityScript>(aEntity); }
	static WireGUID GetStaticGUID() { return "{73C02103-A608-4869-94BC-9DBEE6C3E267}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }
private:
	float myTimer = 0;
};