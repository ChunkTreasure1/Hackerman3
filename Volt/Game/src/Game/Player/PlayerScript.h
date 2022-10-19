#pragma once
#include "Volt/Scene/Entity.h"
#include "Volt/Scripting/ScriptBase.h"
#include "PlayerSM.h"
#include "GEM/gem.h"
#include <Volt/Events/MouseEvent.h>
#include <Volt/Events/GameEvent.h>
#include "Volt/Events/KeyEvent.h"

enum UnlockableAbilities
{
	Swipe,
	Dash,
	Fear,
	BloodLust,
	BuffedArrow
};

class PlayerScript : public Volt::ScriptBase
{
public:
	PlayerScript(const Volt::Entity& aEntity);
	void OnAwake() override;
	void OnStart() override;
	void OnEvent(Volt::Event& e) override;
	void OnUpdate(float aDeltaTime) override;
	void OnStop() override;

	bool Respawn(Volt::OnRespawnEvent& e);
	void Die();

	void ActivateBloodlust();
	void ResetFuryTimer();
	void DecreaseFury(float aDeltaTime);

	bool TryRaycastTarget();
	void MoveTowardsTarget();

	void CanCancelAbility(bool canCancel) { myCancelAbility = canCancel; }

	void UpdateAbilityCooldowns(float aDeltaTime);
	gem::vec3 GetWorldPosFromMouse();

	Volt::Entity GetCameraEntity();

	bool CheckMouseInput(Volt::MouseButtonPressedEvent& e);
	void CheckKeyInput(Volt::KeyPressedEvent& e);

	void GiveAbility(UnlockableAbilities aAbility);
	void SetRespawnPosition(gem::vec3 aRespawnPos);

	bool SetWalkState();

	static Ref<ScriptBase> Create(Volt::Entity aEntity) { return CreateRef<PlayerScript>(aEntity); }
	static WireGUID GetStaticGUID() { return "{91FE5D11-01BC-43B2-BB26-FFFFBE961252}"_guid; };
	WireGUID GetGUID() override { return GetStaticGUID(); }
private:
	gem::vec3 myTargetPosition = {0,0,0};
	gem::vec3 myRespawnPosition = { 0,0,0 };

	gem::vec2 myViewportMousePos;
	gem::vec2 myViewportSize;

	float myTimeUntilFuryDecrease = 0.f;

	float myBloodlustTimer = 0.f;
	float furyDecreaseTimer = 0.f;

	float timeUntillAutoPress = 0;

	Ref<PlayerSM> mySM;
	
	bool myBloodlustActive = false;
	bool myIsDead = false;
	bool myFirstTimeAttackingTarget = true;
	bool myCancelAbility = true;

	Volt::Entity myTargetedEntity;
};