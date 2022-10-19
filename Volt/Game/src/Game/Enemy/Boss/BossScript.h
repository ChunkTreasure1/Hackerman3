#pragma once
#include <Volt/Scripting/ScriptBase.h>
#include "Game/Enemy/Boss/BossSM.h"
#include "Game/Enemy/Boss/AbilityHandler.h"

class BossScript : public Volt::ScriptBase
{
public:
	BossScript(const Volt::Entity& aEntity);
	~BossScript();

	void OnAwake() override;
	void OnStart() override;
	void OnUpdate(float deltaTime) override;
	void OnStop() override;

	int GetStartHP() { return myStartHP; }

	Ref<AbilityHandler> GetAbilityHandler() { return myAbilityHandler; };

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<BossScript>(aEntity); }
	static WireGUID GetStaticGUID() { return "{078DCD34-4077-41FD-BF5B-98BDF197A409}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }
private:
	Ref<BossSM> mySM;
	Ref<AbilityHandler> myAbilityHandler;

	int myStartHP = 0;
};