#include <Volt/Scripting/ScriptBase.h>

SERIALIZE_COMPONENT((struct CompterComponent
{
	bool isPlayerInTrigger = false;

	CREATE_COMPONENT_GUID("{9E4E7DB9-506B-4D00-87CC-CDF13F7F3554}"_guid);
}), CompterComponent);

class CompterScript : public Volt::ScriptBase
{
public:
	CompterScript(Volt::Entity entity);

	void OnTriggerEnter(Volt::Entity entity, bool isTrigger) override;
	void OnTriggerExit(Volt::Entity entity, bool isTrigger) override;

	inline const bool IsPlayerInTrigger() const { return myEntity.GetComponent<CompterComponent>().isPlayerInTrigger; }

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<CompterScript>(aEntity); }
	static WireGUID GetStaticGUID() { return "{706BA89A-0892-4079-AFDC-DD502E92E931}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }

private:
};