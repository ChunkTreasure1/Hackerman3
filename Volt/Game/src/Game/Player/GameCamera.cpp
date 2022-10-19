#include "GameCamera.h"
#include "Volt/Scripting/ScriptRegistry.h"
#include "Volt/Components/Components.h"
GameCamera::GameCamera(Volt::Entity entity)
	: ScriptBase(entity)
{
}

void GameCamera::OnAwake()
{
}

void GameCamera::OnStart()
{
	if (Volt::Entity ent = FindPlayerEntity())
	{
		myTarget = ent;

		gem::vec3 offsetPos = { 1100, 1500, -1100 };
		Volt::TransformComponent& transform = myEntity.GetComponent<Volt::TransformComponent>();
		transform.position = myTarget.GetPosition() + offsetPos;
		transform.rotation = { gem::radians(43.f), gem::radians(-45.f), 0 };
		myEntity.GetComponent<Volt::CameraComponent>().fieldOfView = 31.6f;
	}
}

void GameCamera::OnEvent(Volt::Event& e)
{
}

void GameCamera::OnUpdate(float aDeltaTime)
{
	if (myTarget)
	{
		gem::vec3 pos = GetSmoothStepPos(aDeltaTime);
		myEntity.SetPosition(pos);
	}
}

void GameCamera::OnStop()
{
}

gem::vec3 GameCamera::GetSmoothStepPos(float aDeltaTime)
{
	gem::vec3 offsetPos = { 1100, 1350, -1100 };
	float t = aDeltaTime / myEntity.GetComponent<Volt::CameraComponent>().smoothTime;
	t = t * t * (3.0f - 2.0f * t);
	gem::vec3 resultPos = gem::lerp(myEntity.GetPosition(), myTarget.GetPosition() + offsetPos, t);
	return resultPos;
}

Volt::Entity GameCamera::FindPlayerEntity()
{
	Volt::Entity tempEnt;
	myEntity.GetScene()->GetRegistry().ForEach<Volt::PlayerComponent>([&](Wire::EntityId id, const Volt::PlayerComponent& scriptComp)
		{
			tempEnt = { id, myEntity.GetScene() };
			return;
		});

	return tempEnt;
}

VT_REGISTER_SCRIPT(GameCamera);