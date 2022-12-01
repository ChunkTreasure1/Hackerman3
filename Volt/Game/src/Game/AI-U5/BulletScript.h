#pragma once

#include "StateMachine.h"
#include <Volt/Scripting/ScriptBase.h>

class BulletScript : public Volt::ScriptBase
{
public:
	BulletScript(Volt::Entity entity);

	void OnCollisionEnter(Volt::Entity entity) override;

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<BulletScript>(aEntity); }
	static WireGUID GetStaticGUID() { return "{9FED3102-C830-4CE7-8F0E-430EFF3813BE}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }

private:
};