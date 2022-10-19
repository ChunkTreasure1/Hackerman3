#include "NavMeshAgentTest.h"

#include <Volt/Components/PhysicsComponents.h>
#include <Volt/Components/Components.h>

#include <Volt/Asset/AssetManager.h>
#include <Volt/Input/KeyCodes.h>

#include <Volt/Physics/PhysicsActor.h>
#include <Volt/Physics/PhysicsScene.h>
#include <Volt/Physics/Physics.h>

#include <Volt/Utility/Random.h>

#include <Volt/Core/Application.h>
#include <Volt/Events/MouseEvent.h>

NavMeshAgentTest::NavMeshAgentTest(Volt::Entity entity)
	: Volt::ScriptBase(entity)
{}

void NavMeshAgentTest::OnAwake()
{
}

void NavMeshAgentTest::OnEvent(Volt::Event& e)
{
	Volt::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Volt::KeyPressedEvent>(VT_BIND_EVENT_FN(NavMeshAgentTest::OnKeyPressedEvent));

	if (Volt::Application::Get().IsRuntime())
	{
		dispatcher.Dispatch<Volt::MouseMovedEvent>([&](Volt::MouseMovedEvent& e)
			{
				myViewportMousePos = { e.GetX(), e.GetY() };

				return false;
			});
	}
	else
	{
		dispatcher.Dispatch<Volt::MouseMovedViewportEvent>([&](Volt::MouseMovedViewportEvent& e)
			{
				myViewportMousePos = { e.GetX(), e.GetY() };
				return false;
			});
	}

	dispatcher.Dispatch<Volt::WindowResizeEvent>([&](Volt::WindowResizeEvent& e)
		{
			myViewportSize = { (float)e.GetWidth(), (float)e.GetHeight() };
			return false;
		});

	dispatcher.Dispatch<Volt::ViewportResizeEvent>([&](Volt::ViewportResizeEvent& e)
		{
			myViewportSize = { (float)e.GetWidth(), (float)e.GetHeight() };
			return false;
		});
}

void NavMeshAgentTest::OnUpdate(float aDeltaTime)
{}

void NavMeshAgentTest::OnCollisionEnter(Volt::Entity entity)
{
	VT_CORE_ASSERT(false);
}

gem::vec3 NavMeshAgentTest::GetWorldPosFromMouse()
{
	Volt::Entity cam = GetCameraEntity();
	gem::vec3 dir;

	if (cam.HasComponent<Volt::CameraComponent>())
	{
		dir = cam.GetComponent<Volt::CameraComponent>().camera->ScreenToWorldCoords(myViewportMousePos, myViewportSize);
	}

	gem::vec3 camPos = cam.GetPosition();
	gem::vec3 targetPos;
	targetPos = camPos;

	while (targetPos.y > myEntity.GetPosition().y)
	{
		gem::vec3 deltaPos = targetPos;
		if (deltaPos.y > (targetPos + dir).y)
		{
			targetPos += dir;
			continue;
		}
		break;
	}

	return { targetPos.x, 0.f, targetPos.z };
}

Volt::Entity NavMeshAgentTest::GetCameraEntity()
{
	Volt::Entity tempEnt;

	myEntity.GetScene()->GetRegistry().ForEach<Volt::CameraComponent>([&](Wire::EntityId id, const Volt::CameraComponent& scriptComp)
		{
			tempEnt = { id, myEntity.GetScene() };
			return;
		});

	return tempEnt;
}

bool NavMeshAgentTest::OnKeyPressedEvent(Volt::KeyPressedEvent& e)
{
	switch (e.GetKeyCode())
	{
	case VT_KEY_SPACE:
	{
		auto target = GetWorldPosFromMouse();

		if (myEntity.HasComponent<Volt::NavMeshAgentComponent>())
		{
			myEntity.GetComponent<Volt::NavMeshAgentComponent>().agent.SetTarget(target);
			
			std::stringstream ss;
			ss << "TARGET: " << target.x << ", " << target.y << ", " << target.z;

			VT_INFO(ss.str());
		}
		break;
	}
	case VT_KEY_X:
	{
		if (myEntity.HasComponent<Volt::NavMeshAgentComponent>())
		{
			auto& agent = myEntity.GetComponent<Volt::NavMeshAgentComponent>().agent;
			agent.SetSpeed(agent.GetSpeed() + 50.f);
		}
		break;
	}
	case VT_KEY_Z:
	{
		if (myEntity.HasComponent<Volt::NavMeshAgentComponent>())
		{
			auto& agent = myEntity.GetComponent<Volt::NavMeshAgentComponent>().agent;
			agent.SetSpeed(agent.GetSpeed() - 50.f);
		}
		break;
	}
	}

	return false;
}

VT_REGISTER_SCRIPT(NavMeshAgentTest);