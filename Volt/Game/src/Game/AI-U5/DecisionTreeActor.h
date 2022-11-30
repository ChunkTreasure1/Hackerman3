#pragma once

#include <Volt/Scripting/ScriptBase.h>

SERIALIZE_COMPONENT((struct AIU5DecisionActorComponent
{
	CREATE_COMPONENT_GUID("{52ACFFF3-BD0A-490B-B096-04ECA5CA8064}"_guid);
}), AIU5DecisionActorComponent);

class DecisionTreeActor : public Volt::ScriptBase
{
public:
	DecisionTreeActor(Volt::Entity entity);

	void OnStart() override;
	void OnUpdate(float aDeltaTime) override;

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<DecisionTreeActor>(aEntity); }
	static WireGUID GetStaticGUID() { return "{5E3DCF3E-690E-4CB7-806B-231BCC1270E0}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }

private:
};