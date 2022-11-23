#include <Volt/Scripting/ScriptBase.h>

SERIALIZE_COMPONENT((struct SeperationControllerComponent
{
	PROPERTY(Name = Speed) float speed = 100.f;
	PROPERTY(Name = Threshold) float threshold = 100.f;
	PROPERTY(Name = Speed Decay) float speedDecay = 100.f;

	CREATE_COMPONENT_GUID("{F7B7949B-4CBD-43C6-A3F3-BC4959276D60}"_guid);
}), SeperationControllerComponent);

class SeparationController : public Volt::ScriptBase
{
public:
	SeparationController(Volt::Entity entity);

	void OnStart() override;
	void OnUpdate(float aDeltaTime) override;

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<SeparationController >(aEntity); }
	static WireGUID GetStaticGUID() { return "{1F2A3824-9C15-4396-9731-3D708C3CF58E}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }

private:
};