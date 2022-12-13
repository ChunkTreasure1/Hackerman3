#pragma once

#include "StateMachine.h"
#include <Volt/Scripting/ScriptBase.h>

class BulletScript : public Volt::ScriptBase
{
public:
	BulletScript(Volt::Entity entity);

	void OnAwake() override;
	void OnCollisionEnter(Volt::Entity entity) override;


	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<BulletScript>(aEntity); }
	static WireGUID GetStaticGUID() { return "{D768DA92-9D4E-4A53-A6CE-1E9F9615A358}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }

private:
};