#pragma once
#include <Volt/Scripting/ScriptBase.h>

class ChampionAbilityScript : public Volt::ScriptBase
{
public:
	ChampionAbilityScript(const Volt::Entity& aEntity);
	void OnAwake()  override;
	void OnStart() override;
	void OnUpdate(float aDeltaTime)  override;
	void OnStop()  override;

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<ChampionAbilityScript>(aEntity); }
	static WireGUID GetStaticGUID() { return "{BF94597A-7CC4-494E-95C0-93A180E49303}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }
private:
};