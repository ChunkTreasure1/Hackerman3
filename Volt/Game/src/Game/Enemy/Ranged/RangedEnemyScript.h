#pragma once
#include <Volt/Scripting/ScriptBase.h>
#include "Game/Enemy/Ranged/RangedEnemySM.h"

class RangedEnemyScript : public Volt::ScriptBase
{
public:
	RangedEnemyScript(const Volt::Entity& aEntity);
	void OnAwake()  override;
	void OnStart() override;
	void OnUpdate(float aDeltaTime)  override;
	void OnStop()  override;

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<RangedEnemyScript>(aEntity); }
	static WireGUID GetStaticGUID() { return "{061B0A1E-A7C4-4372-B973-D86451326BCB}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }
private:
	Ref<RangedEnemySM> mySM;
};