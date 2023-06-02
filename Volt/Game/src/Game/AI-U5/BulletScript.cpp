#include "BulletScript.h"
#include "PollingStationU5.h"
#include "StateMachineActor.h"

#include <Volt/Log/Log.h>

VT_REGISTER_SCRIPT(BulletScript);

BulletScript::BulletScript(Volt::Entity entity)
	: Volt::ScriptBase(entity)
{}

void BulletScript::OnAwake()
{
	VT_CORE_INFO("Bullet created!");
}

void BulletScript::OnCollisionEnter(Volt::Entity entity)
{
	constexpr float damage = 20.f;

	if (entity.HasComponent<AIU5HealthComponent>())
	{
		auto& health = entity.GetComponent<AIU5HealthComponent>().currentHealth;
		health -= damage;
		VT_CORE_INFO("Health {0}", health);

		if (entity.HasComponent<AIU5StateActorComponent>())
		{
			entity.GetComponent<AIU5StateActorComponent>().Attacked = true;
		}
	}

	myEntity.GetScene()->RemoveEntity(myEntity);
}
