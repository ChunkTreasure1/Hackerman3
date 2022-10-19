#pragma once
#include "Volt/Scripting/ScriptBase.h"

class DestructableObject : public Volt::ScriptBase 
{
public:
	DestructableObject(Volt::Entity entity);
	~DestructableObject() override = default;

	void OnTriggerEnter(Volt::Entity entity, bool isTrigger) override;

	void ExpolodeObject(gem::vec3 aForceDir);

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<DestructableObject>(aEntity); }
	static WireGUID GetStaticGUID() { return "{33B3A56F-B960-4476-BF80-90E6621C56DF}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }

private:
	bool hasExploded = false;
};