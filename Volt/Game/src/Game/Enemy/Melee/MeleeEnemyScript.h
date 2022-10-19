#pragma once
#include <Volt/Scripting/ScriptBase.h>
#include "Game/Enemy/Melee/MeleeEnemySM.h"

class MeleeEnemyScript : public Volt::ScriptBase
{
public:
	MeleeEnemyScript(const Volt::Entity& aEntity);
	void OnAwake()  override;
	void OnStart() override;
	void OnUpdate(float aDeltaTime)  override;
	void OnStop()  override;
	void OnTriggerEnter(Volt::Entity entity, bool isTrigger) override {};

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<MeleeEnemyScript>(aEntity); }
	static WireGUID GetStaticGUID() { return "{7F3C3927-0D68-435D-B202-AE15C4D291BA}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }
private:
	Ref<MeleeEnemySM> mySM;
};