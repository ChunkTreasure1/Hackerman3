#pragma once
#include <Volt/Scripting/ScriptBase.h>

class GasShieldAbilityScript : public Volt::ScriptBase
{
public:
	GasShieldAbilityScript(const Volt::Entity& aEntity);
	void OnAwake()  override;
	void OnStart() override;
	void OnUpdate(float aDeltaTime)  override;
	void OnStop()  override;

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<GasShieldAbilityScript>(aEntity); }
	static WireGUID GetStaticGUID() { return "{6F51374B-E7B8-445C-BB68-8F12081E438B}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }
private:
};