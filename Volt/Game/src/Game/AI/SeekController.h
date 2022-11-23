#include <Volt/Scripting/ScriptBase.h>

SERIALIZE_COMPONENT((struct SeekControllerComponent
{
	PROPERTY(Name = Speed) float speed = 100.f;

	CREATE_COMPONENT_GUID("{4ED94E8F-1D78-4236-9E95-15D855BE79BB}"_guid);
}), SeekControllerComponent);

class SeekController : public Volt::ScriptBase
{
public:
	SeekController(Volt::Entity entity);

	void OnStart() override;
	void OnUpdate(float aDeltaTime) override;
	void OnTriggerEnter(Volt::Entity entity, bool isTrigger) override;

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<SeekController >(aEntity); }
	static WireGUID GetStaticGUID() { return "{6674DDD9-0AC2-4DFB-BCA2-8EBB763C5AA1}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }

private:
	void CreateColl();

	bool myExplode = false;
};