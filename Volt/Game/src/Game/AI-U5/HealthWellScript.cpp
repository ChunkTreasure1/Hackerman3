#include "HealthWellScript.h"

#include "PollingStationU5.h"

VT_REGISTER_SCRIPT(HealthWellScript);

HealthWellScript::HealthWellScript(Volt::Entity entity)
	: Volt::ScriptBase(entity)
{}

void HealthWellScript::OnUpdate(float aDeltaTime)
{
	myTimer -= aDeltaTime;
	if (myTimer <= 0.f)
	{
		myTimer = 1.f;
		for (auto& entity : myEntities)
		{
			if (entity.HasComponent<AIU5HealthComponent>())
			{
				auto& healthComp = entity.GetComponent<AIU5HealthComponent>();
				healthComp.currentHealth += myEntity.GetComponent<HealthWellComponent>().healRate;
				if (healthComp.currentHealth > healthComp.health)
				{
					healthComp.currentHealth = healthComp.health;
				}
			}
		}
	}
}

void HealthWellScript::OnTriggerEnter(Volt::Entity entity, bool isTrigger)
{
	if (std::find(myEntities.begin(), myEntities.end(), entity) != myEntities.end())
	{
		return;
	}

	myEntities.emplace_back(entity);
}

void HealthWellScript::OnTriggerExit(Volt::Entity entity, bool isTrigger)
{
	if (auto it = std::find(myEntities.begin(), myEntities.end(), entity); it != myEntities.end())
	{
		myEntities.erase(it);
	}
}