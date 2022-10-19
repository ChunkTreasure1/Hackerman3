#include "PlayerScript.h"
#include "Volt/Log/Log.h"
#include "Volt/Input/Input.h"
#include "Volt/Input/KeyCodes.h"
#include "Volt/Input/MouseButtonCodes.h"
#include "Volt/Scripting/ScriptRegistry.h"
#include "Volt/Components/Components.h"
#include "Volt/Asset/AssetManager.h"
#include "Volt/Core/Application.h"
#include "Volt/Animation/AnimationManager.h"
#include "Volt/UI/UIFunctionRegistry.h"
#include "Volt/Events/GameEvent.h"
#include "Volt/Physics/PhysicsScene.h"
#include "Volt/Physics/Physics.h"

VT_REGISTER_SCRIPT(PlayerScript)

PlayerScript::PlayerScript(const Volt::Entity& aEntity)
	:ScriptBase(aEntity),
	myPrimaryAbility(aEntity), myArrowAbility(aEntity),
	mySwipeAbility(aEntity), myDashAbility(aEntity),
	myFearAbility(aEntity), myBloodlustAbility(aEntity)
{
}

void PlayerScript::OnAwake()
{
	
	mySM = CreateRef<PlayerSM>(myEntity);

	auto& animComp = myEntity.AddComponent<Volt::AnimatedCharacterComponent>();
	animComp.animatedCharacter = Volt::AssetManager::Get().GetAssetHandleFromPath("Assets/Character/CHA_Player.vtchr");

	animComp.currentAnimation = 6;
	animComp.currentStartTime = Volt::AnimationManager::globalClock;

	mySM->CreateStates();
} 

void PlayerScript::OnStart()
{
	myRespawnPosition = myEntity.GetPosition();
}

void PlayerScript::OnEvent(Volt::Event& e)
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

	dispatcher.Dispatch<Volt::OnRespawnEvent>(VT_BIND_EVENT_FN(PlayerScript::Respawn));

	if (!myIsDead)
	{
		dispatcher.Dispatch<Volt::KeyPressedEvent>([&](Volt::KeyPressedEvent& e)
			{
				CheckKeyInput(e);
				return false;
			});
		dispatcher.Dispatch<Volt::MouseButtonPressedEvent>(VT_BIND_EVENT_FN(PlayerScript::CheckMouseInput));
	}
}

void PlayerScript::OnUpdate(float aDeltaTime)
{
	mySM->Update(aDeltaTime);
	UpdateAbilityCooldowns(aDeltaTime);

	if (myTimeUntilFuryDecrease <= 0)
	{
		DecreaseFury(aDeltaTime);
	}
	else
	{
		myTimeUntilFuryDecrease -= aDeltaTime;
	}

	if (mySM->GetActiveState() != ePlayerState::WALK)
	{
		myTargetPosition = myEntity.GetPosition();
	}

	if (myEntity.GetComponent<Volt::HealthComponent>().health <= 0 && !myIsDead)
	{
		Die();
	}

	if (myBloodlustTimer > 0) 
	{
		auto& comp = myEntity.GetComponent<Volt::PlayerComponent>();
		comp.currentFury = comp.maxFury;
		myBloodlustTimer -= aDeltaTime;
	}
	else 
	{
		myBloodlustActive = false;
	}

	if (!myTargetedEntity.IsNull())
	{
		MoveTowardsTarget();
	}

	if (Volt::Input::IsMouseButtonPressed(0)) 
	{
		timeUntillAutoPress -= aDeltaTime;
		if (timeUntillAutoPress <= 0 && myCancelAbility == true) 
		{
			timeUntillAutoPress = 0;
			SetWalkState();
		}
	}
	else
	{
		timeUntillAutoPress = 0.3f;
	}
}

void PlayerScript::OnStop()
{
}

bool PlayerScript::Respawn(Volt::OnRespawnEvent& e)
{
	myEntity.SetPosition(myRespawnPosition);
	myEntity.GetComponent<Volt::HealthComponent>().health = 120;
	myEntity.GetComponent<Volt::PlayerComponent>().currentFury = 0;
	myIsDead = false;
	mySM->SetState(ePlayerState::IDLE);
	return false;
}

void PlayerScript::Die()
{
	mySM->SetState(ePlayerState::DIE);
	Volt::UIFunctionRegistry::Execute("OnGameOver");
	myIsDead = true;
}

void PlayerScript::ActivateBloodlust()
{
	myBloodlustTimer = AbilityData::BloodLustData->buffTime;
	myBloodlustActive = true;
}

void PlayerScript::ResetFuryTimer()
{
	myTimeUntilFuryDecrease = myEntity.GetComponent<Volt::PlayerComponent>().timeUntilFuryDecrese;
}

void PlayerScript::DecreaseFury(float aDeltaTime)
{
	if (furyDecreaseTimer > 0)
	{
		furyDecreaseTimer -= aDeltaTime;
	}
	else
	{
		auto& playerComp = myEntity.GetComponent<Volt::PlayerComponent>();
		playerComp.currentFury -= playerComp.furyDecreaseAmount;
		furyDecreaseTimer = 0.5f;
		if (playerComp.currentFury < 0)
		{
			playerComp.currentFury = 0;
		}
	}
}

bool PlayerScript::TryRaycastTarget()
{
	Volt::Entity cam = GetCameraEntity();
	gem::vec3 dir;

	if (cam.HasComponent<Volt::CameraComponent>())
	{
		dir = cam.GetComponent<Volt::CameraComponent>().camera->ScreenToWorldCoords(myViewportMousePos, myViewportSize);
	}
	
	Volt::RaycastHit hit;
	if (Volt::Physics::GetScene()->Raycast(cam.GetPosition(), dir, 100000.f, &hit, 3))
	{
		if (myTargetedEntity == Volt::Entity{ hit.hitEntity, myEntity.GetScene() }) return true;

		myTargetedEntity = Volt::Entity{hit.hitEntity, myEntity.GetScene()};
		
		if (mySM->GetActiveState() != ePlayerState::WALK)
		{
			mySM->SetState(ePlayerState::WALK);
		}

		mySM->GetPlayerState<PlayerWalkState>(ePlayerState::WALK)->SetTargetPos(myTargetedEntity.GetPosition());
		myFirstTimeAttackingTarget = true;

		return true;
	}

	//Other Interactables!
	//if (Volt::Physics::GetScene()->Raycast(cam.GetPosition(), dir, 100000.f, &hit, 3))
	//{
	//	myTargetedEntity = Volt::Entity{ hit.hitEntity, myEntity.GetScene() };
	//}

	//if (Volt::Physics::GetScene()->Raycast(cam.GetPosition(), dir, 100000.f, &hit, 3))
	//{
	//	myTargetedEntity = Volt::Entity{ hit.hitEntity, myEntity.GetScene() };
	//}
	myTargetedEntity = Volt::Entity{ 0, nullptr };
	return false;
}

void PlayerScript::MoveTowardsTarget()
{
	if (myTargetedEntity.IsNull())
	{
		return;
	}

	gem::vec3 resultPos;
	
	gem::vec3 dir = myTargetedEntity.GetPosition() - myEntity.GetPosition();
	float targetCollRadius = myTargetedEntity.GetComponent<Volt::CapsuleColliderComponent>().radius;
	float attackDist = myEntity.GetComponent<Volt::PlayerComponent>().attackStopDist;

	gem::vec3 stopDist = dir / (targetCollRadius + attackDist);

	resultPos = myEntity.GetPosition() + (dir - stopDist);

	resultPos.y = myEntity.GetPosition().y;

	mySM->GetPlayerState<PlayerWalkState>(ePlayerState::WALK)->SetTargetPos(resultPos);
}

void PlayerScript::UpdateAbilityCooldowns(float aDeltaTime)
{
	myPrimaryAbility.Update(aDeltaTime);
	myArrowAbility.Update(aDeltaTime);
	myFearAbility.Update(aDeltaTime);
	myDashAbility.Update(aDeltaTime);
	mySwipeAbility.Update(aDeltaTime);
	myBloodlustAbility.Update(aDeltaTime);
}

gem::vec3 PlayerScript::GetWorldPosFromMouse()
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

	return {targetPos.x, myEntity.GetPosition().y, targetPos.z};
}

Volt::Entity PlayerScript::GetCameraEntity()
{
	Volt::Entity tempEnt;

	myEntity.GetScene()->GetRegistry().ForEach<Volt::CameraComponent>([&](Wire::EntityId id, const Volt::CameraComponent& scriptComp)
		{
			tempEnt = { id, myEntity.GetScene() };
			return;
		});

	return tempEnt;
}

bool PlayerScript::CheckMouseInput(Volt::MouseButtonPressedEvent& e)
{
	if (!Volt::Application::Get().IsRuntime() && !e.overViewport) { return false; }

	if (mySM->GetActiveState() == ePlayerState::WALK || mySM->GetActiveState() == ePlayerState::IDLE)
	{
		if (e.GetMouseButton() == VT_MOUSE_BUTTON_LEFT && Volt::Input::IsKeyDown(VT_KEY_LEFT_SHIFT) && myPrimaryAbility.IsCastable())
		{
			myPrimaryAbility.SetDir(GetWorldPosFromMouse());
			myPrimaryAbility.CastAfterTime(0.3f);
			mySM->SetState(ePlayerState::PRIMARY);
			CanCancelAbility(false);
			return false;
		}
		else if (e.GetMouseButton() == VT_MOUSE_BUTTON_RIGHT && myArrowAbility.IsCastable())
		{
			myArrowAbility.SetDir(GetWorldPosFromMouse());
			myArrowAbility.CastAfterTime(0.2f);
			mySM->SetState(ePlayerState::ARROW);
			CanCancelAbility(false);
			return false;
		}
	}

	if (e.GetMouseButton() == VT_MOUSE_BUTTON_LEFT && !Volt::Input::IsKeyDown(VT_KEY_LEFT_SHIFT) && myCancelAbility == true)
	{
		if (TryRaycastTarget())
		{
			return false;
		}
		SetWalkState();
	}
	else if (e.GetMouseButton() == VT_MOUSE_BUTTON_MIDDLE && myCancelAbility == true)
	{
		SetWalkState();
	}



	return false;
}

void PlayerScript::GiveAbility(UnlockableAbilities aAbility)
{
	switch (aAbility)
	{
	case Swipe:
		mySwipeAbility.SetUnlocked(true);
		break;
	case Dash:
		myDashAbility.SetUnlocked(true);
		break;
	case Fear:
		myFearAbility.SetUnlocked(true);
		break;
	case BloodLust:
		myBloodlustAbility.SetUnlocked(true);
		break;
	case BuffedArrow:
		myEntity.GetComponent<Volt::PlayerComponent>().isBuffed = true;
		break;
	default:
		VT_CORE_INFO("Eyo wtf?");
		break;
	}
}

void PlayerScript::SetRespawnPosition(gem::vec3 aRespawnPos)
{
	myRespawnPosition = aRespawnPos;
}


void PlayerScript::CheckKeyInput(Volt::KeyPressedEvent& e)
{
	//DEBUG TEST INPUTS (REMOVE LATER)
	{
		if (e.GetKeyCode() == VT_KEY_F3)
		{
			myEntity.GetComponent<Volt::PlayerComponent>().isBuffed = !myEntity.GetComponent<Volt::PlayerComponent>().isBuffed;
		}

		if (e.GetKeyCode() == VT_KEY_F)
		{
			myEntity.GetComponent<Volt::PlayerComponent>().currentFury += 20;
		}

		if (e.GetKeyCode() == VT_KEY_G)
		{
			myTimeUntilFuryDecrease = myEntity.GetComponent<Volt::PlayerComponent>().timeUntilFuryDecrese;
		}

		if (e.GetKeyCode() == VT_KEY_U)
		{
			mySwipeAbility.SetUnlocked(true);
			myDashAbility.SetUnlocked(true);
			myFearAbility.SetUnlocked(true);
			myBloodlustAbility.SetUnlocked(true);
		}
	}

	if (mySM->GetActiveState() == ePlayerState::IDLE || mySM->GetActiveState() == ePlayerState::WALK)
	{
		auto& playerComp = myEntity.GetComponent<Volt::PlayerComponent>();
		switch (e.GetKeyCode())
		{
		case VT_KEY_A:
		case VT_KEY_1:
			VT_CORE_INFO("Key 1");
			if (mySwipeAbility.IsCastable() && mySwipeAbility.isUnlocked() && playerComp.currentFury >= AbilityData::SwipeData->cost)
			{
				CanCancelAbility(false);
				mySwipeAbility.SetDir(GetWorldPosFromMouse());
				mySwipeAbility.CastAfterTime(0.5f);

				if (mySM->GetActiveState() == ePlayerState::WALK)
				{
					mySwipeAbility.InitVFXAfterTime(myEntity.GetPosition(), {0, myEntity.GetRotation().y, 0}, { 1,1,1 }, true, 0.4f, 0.5f, "Assets/Prefabs/Swipe.vtprefab");
					mySM->GetPlayerState<PlayerSwipeState>(ePlayerState::SWIPE)->PlayerIsMoving(true);
				}
				else
				{
					mySwipeAbility.InitVFXAfterTime(myEntity.GetPosition(), myEntity.GetRotation(), { 1,1,1 }, false, 0.4f, 0.5f, "Assets/Prefabs/Swipe.vtprefab");
					mySM->GetPlayerState<PlayerSwipeState>(ePlayerState::SWIPE)->PlayerIsMoving(false);
				}
				mySM->SetState(ePlayerState::SWIPE);
				
				playerComp.currentFury -= AbilityData::SwipeData->cost;
			}
			break;
		case VT_KEY_S:
		case VT_KEY_2:
			VT_CORE_INFO("Key 2");
			if (myDashAbility.IsCastable() && myDashAbility.isUnlocked() && playerComp.currentFury >= AbilityData::DashData->cost)
			{
				CanCancelAbility(false);
				myDashAbility.SetDir(GetWorldPosFromMouse());
				myDashAbility.CastAfterTime(0.2f);
				mySM->SetState(ePlayerState::DASH);
				playerComp.currentFury -= AbilityData::DashData->cost;
			}
			break;
		case VT_KEY_D:
		case VT_KEY_3:
			VT_CORE_INFO("Key 3");
			if (myFearAbility.IsCastable() && myFearAbility.isUnlocked() && playerComp.currentFury >= AbilityData::FearData->cost)
			{
				CanCancelAbility(false);
				myFearAbility.CastAfterTime(0.8f);
				myFearAbility.InitVFXAfterTime(myEntity.GetPosition(), myEntity.GetRotation(), { 1,1,1 }, false, 0.8f, AbilityData::FearData->fearTime, "Assets/Prefabs/Fear.vtprefab");
				mySM->SetState(ePlayerState::FEAR);
				playerComp.currentFury -= AbilityData::FearData->cost;
			}
			break;
		case VT_KEY_F:
		case VT_KEY_4:
			VT_CORE_INFO("Key 4");
			if (myBloodlustAbility.IsCastable() && myBloodlustAbility.isUnlocked() && playerComp.currentFury >= AbilityData::BloodLustData->cost)
			{
				CanCancelAbility(false);
				myBloodlustAbility.Cast();
				myBloodlustAbility.InitVFX(myEntity.GetPosition(), myEntity.GetRotation(), { 1,1,1 }, false, 0.5f, "Assets/Prefabs/Bloodlust.vtprefab");
				mySM->SetState(ePlayerState::BLOODLUST);
				playerComp.currentFury -= AbilityData::BloodLustData->cost;
			}
			break;
		default:
			break;
		}
	}
}



bool PlayerScript::SetWalkState()
{
	if (TryRaycastTarget())
	{
		return false;
	}
	myTargetPosition = GetWorldPosFromMouse();
	//myEntity.GetComponent<Volt::NavMeshAgentComponent>().agent.SetTarget(myTargetPosition);

	Volt::Entity moveIndicator = myEntity.GetScene()->CreateEntity();
	auto& comp = moveIndicator.AddComponent<Volt::MeshComponent>();

	if (auto mesh = Volt::AssetManager::GetAsset<Volt::Mesh>("Assets/Meshes/Primitives/Sphere.vtmesh"))
	{
		comp.handle = mesh->handle;
	}

	moveIndicator.GetComponent<Volt::TransformComponent>().position = myTargetPosition;
	moveIndicator.GetComponent<Volt::TransformComponent>().scale *= 0.5f;

	myEntity.GetScene()->RemoveEntity(moveIndicator, 1.f);

	if (mySM->GetActiveState() != ePlayerState::WALK)
	{
		mySM->SetState(ePlayerState::WALK);
	}
	mySM->GetPlayerState<PlayerWalkState>(ePlayerState::WALK)->SetTargetPos(myTargetPosition);

	myTargetedEntity = Volt::Entity{ 0, nullptr };
	myFirstTimeAttackingTarget = true;

	return false;
}
