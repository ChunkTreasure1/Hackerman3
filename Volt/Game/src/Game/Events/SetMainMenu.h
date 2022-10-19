#pragma once
#include "Volt/Scripting/ScriptBase.h"

class SetMainMenu : public Volt::ScriptBase
{
public:
	SetMainMenu(Volt::Entity entity);
	~SetMainMenu() override = default;

	void OnStart() override;

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<SetMainMenu>(aEntity); }
	static WireGUID GetStaticGUID() { return "{83187B17-6E3F-4DBF-9454-6B41E49869E1}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }
};