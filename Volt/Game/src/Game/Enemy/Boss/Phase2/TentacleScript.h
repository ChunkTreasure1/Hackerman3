#pragma once
#include <Volt/Scripting/ScriptBase.h>
#include "Game/Abilities/AbilityBase.h"
#include "TentacleSM.h"

class TentacleScript : public Volt::ScriptBase
{
public:
	TentacleScript(const Volt::Entity& aEntity);
	~TentacleScript();

	void OnAwake() override;
	void OnStart() override;
	void OnUpdate(float deltaTime) override;
	void OnStop() override;

	Ref<AbilityBase> GetLineSlam() { return myLineSlam; }

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<TentacleScript>(aEntity); }
	static WireGUID GetStaticGUID() { return "{A1D47277-97B2-4117-9E64-3ACB0DDD2950}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }

private:
	int myStartHealth = 0;
	Ref<AbilityBase> myLineSlam;
	Ref<TentacleSM> mySM;
};