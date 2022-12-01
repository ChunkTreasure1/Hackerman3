#include "BulletScript.h"

#include "PollingStationU5.h"

VT_REGISTER_SCRIPT(BulletScript);

BulletScript::BulletScript(Volt::Entity entity)
	: Volt::ScriptBase(entity)
{}

void BulletScript::OnCollisionEnter(Volt::Entity entity)
{
	constexpr float damage = 10.f;

	if (entity.HasComponent<AIU5HealthComponent>())
	{
		entity.GetComponent<AIU5HealthComponent>().currentHealth -= damage;
	}

	myEntity.GetScene()->RemoveEntity(myEntity);
}
