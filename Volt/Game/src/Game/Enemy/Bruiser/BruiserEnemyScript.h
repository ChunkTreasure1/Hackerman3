#pragma once
#include <Volt/Scripting/ScriptBase.h>
#include "Game/Enemy/Bruiser/BruiserEnemySM.h"

class BruiserEnemyScript : public Volt::ScriptBase
{
public:
	BruiserEnemyScript(const Volt::Entity& aEntity);
	void OnAwake()  override;
	void OnStart() override;
	void OnUpdate(float aDeltaTime)  override;
	void OnStop()  override;

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<BruiserEnemyScript>(aEntity); }
	static WireGUID GetStaticGUID() { return "{59CF1909-EEA5-42E0-BF0D-39F541AA83B7}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }
private:
	Ref<BruiserEnemySM> mySM;
};