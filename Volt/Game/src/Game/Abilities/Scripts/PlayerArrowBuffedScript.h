#pragma once
#include "Volt/Scene/Entity.h"
#include "Volt/Scripting/ScriptBase.h"
#include "Volt/Scripting/ScriptRegistry.h"
#include "Volt/Log/Log.h"
#include "Volt/Components/Components.h"

class PlayerArrowBuffedScript : public Volt::ScriptBase
{
public:
	PlayerArrowBuffedScript(const Volt::Entity& aEntity);

	void OnTriggerEnter(Volt::Entity entity, bool isTrigger) override;

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<PlayerArrowBuffedScript>(aEntity); }
	static WireGUID GetStaticGUID() { return "{60CE710F-2244-42DB-986F-5F1E70002822}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }

private:
	void OnAwake() override;
	void OnUpdate(float aDeltaTime) override;

};