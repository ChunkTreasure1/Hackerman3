#include "PlayerController.h"

#include <Volt/Input/Input.h>
#include <Volt/Input/KeyCodes.h>
#include <Volt/Input/MouseButtonCodes.h>

#include <Volt/Components/Components.h>

VT_REGISTER_SCRIPT(PlayerController)

PlayerController::PlayerController(Volt::Entity entity)
	: Volt::ScriptBase(entity)
{}

void PlayerController::OnStart()
{
	myEntity.GetScene()->GetRegistry().ForEach<Volt::CameraComponent>([&](Wire::EntityId id, const Volt::CameraComponent& scriptComp)
		{
			myCameraEntity = { id, myEntity.GetScene() };
		});
}

void PlayerController::OnUpdate(float aDeltaTime)
{
	constexpr float walkSpeed = 300.f;

	float distToTarget = gem::length(myTargetPosition - myEntity.GetPosition());
	if (distToTarget > 40)
	{
		gem::vec3 dir = gem::normalize(myTargetPosition - myEntity.GetComponent<Volt::TransformComponent>().position);
		const gem::vec3 pos = myEntity.GetPosition() + dir * walkSpeed * aDeltaTime;
		
		myEntity.SetPosition({ pos.x, myEntity.GetPosition().y, pos.z });
	}
}

void PlayerController::OnEvent(Volt::Event& e)
{
	Volt::EventDispatcher dispatcher(e);

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

	dispatcher.Dispatch<Volt::MouseButtonPressedEvent>(VT_BIND_EVENT_FN(PlayerController::CheckMouseInput));

}

bool PlayerController::CheckMouseInput(Volt::MouseButtonPressedEvent& e)
{
	if (e.GetMouseButton() == VT_MOUSE_BUTTON_LEFT)
	{
		UpdateWalkPosition();
	}

	return false;
}

gem::vec3 PlayerController::GetWorldPosFromMouse()
{
	gem::vec3 dir;

	if (myCameraEntity.HasComponent<Volt::CameraComponent>())
	{
		dir = myCameraEntity.GetComponent<Volt::CameraComponent>().camera->ScreenToWorldCoords(myViewportMousePos, myViewportSize);
	}

	gem::vec3 camPos = myCameraEntity.GetPosition();
	gem::vec3 targetPos;
	targetPos = camPos;

	//DETTA SKA BORT NÄR PATHFINDING ÄR KLAR!!!!!!! 
	{
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
	}

	return { targetPos.x, myEntity.GetPosition().y, targetPos.z };
}

void PlayerController::UpdateWalkPosition()
{
	myTargetPosition = GetWorldPosFromMouse();
}

