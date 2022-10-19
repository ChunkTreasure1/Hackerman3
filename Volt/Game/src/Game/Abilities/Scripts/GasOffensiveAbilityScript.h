#pragma once
#include <Volt/Scripting/ScriptBase.h>

class GasOffensiveAbilityScript : public Volt::ScriptBase
{
public:
	GasOffensiveAbilityScript(const Volt::Entity& aEntity);
	void OnAwake()  override;
	void OnStart() override;
	void OnUpdate(float aDeltaTime)  override;
	void OnStop()  override;

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<GasOffensiveAbilityScript>(aEntity); }
	static WireGUID GetStaticGUID() { return "{986D0E34-1D0D-4420-B4C4-690753B24AE4}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }
private:
};