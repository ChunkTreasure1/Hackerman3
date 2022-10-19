#include "PlayerSM.h"

PlayerSM::PlayerSM(const Volt::Entity& aEntity)
	:StateMachineBase(aEntity)
{
}

void PlayerSM::CreateStates()
{
	Ref<StateBase<ePlayerState>> idle = CreateRef<PlayerIdleState>(myEntity);
	Ref<StateBase<ePlayerState>> walk = CreateRef<PlayerWalkState>(myEntity);
	Ref<StateBase<ePlayerState>> die = CreateRef<PlayerDeathState>(myEntity);
	Ref<StateBase<ePlayerState>> primary = CreateRef<PlayerPrimaryState>(myEntity);
	Ref<StateBase<ePlayerState>> arrow = CreateRef<PlayerArrowState>(myEntity);
	Ref<StateBase<ePlayerState>> swipe = CreateRef<PlayerSwipeState>(myEntity);
	Ref<StateBase<ePlayerState>> dash = CreateRef<PlayerDashState>(myEntity);
	Ref<StateBase<ePlayerState>> fear = CreateRef<PlayerFearState>(myEntity);
	Ref<StateBase<ePlayerState>> bloodLust = CreateRef<PlayerBloodLustState>(myEntity);

	myStates.insert({ ePlayerState::IDLE, idle });
	myStates.insert({ ePlayerState::WALK, walk });
	myStates.insert({ ePlayerState::DIE , die });
	myStates.insert({ ePlayerState::PRIMARY, primary });
	myStates.insert({ ePlayerState::ARROW, arrow });
	myStates.insert({ ePlayerState::SWIPE, swipe });
	myStates.insert({ ePlayerState::DASH, dash });
	myStates.insert({ ePlayerState::FEAR, fear });
	myStates.insert({ ePlayerState::BLOODLUST, bloodLust });

	myStates[ePlayerState::IDLE]->AddTransition(ePlayerState::WALK);
	myStates[ePlayerState::IDLE]->AddTransition(ePlayerState::DIE);
	myStates[ePlayerState::IDLE]->AddTransition(ePlayerState::PRIMARY);
	myStates[ePlayerState::IDLE]->AddTransition(ePlayerState::ARROW);
	myStates[ePlayerState::IDLE]->AddTransition(ePlayerState::SWIPE);
	myStates[ePlayerState::IDLE]->AddTransition(ePlayerState::DASH);
	myStates[ePlayerState::IDLE]->AddTransition(ePlayerState::FEAR);
	myStates[ePlayerState::IDLE]->AddTransition(ePlayerState::BLOODLUST);

	myStates[ePlayerState::WALK]->AddTransition(ePlayerState::IDLE);
	myStates[ePlayerState::WALK]->AddTransition(ePlayerState::DIE);
	myStates[ePlayerState::WALK]->AddTransition(ePlayerState::PRIMARY);
	myStates[ePlayerState::WALK]->AddTransition(ePlayerState::ARROW);
	myStates[ePlayerState::WALK]->AddTransition(ePlayerState::SWIPE);
	myStates[ePlayerState::WALK]->AddTransition(ePlayerState::DASH);
	myStates[ePlayerState::WALK]->AddTransition(ePlayerState::FEAR);
	myStates[ePlayerState::WALK]->AddTransition(ePlayerState::BLOODLUST);

	myStates[ePlayerState::PRIMARY]->AddTransition(ePlayerState::IDLE);
	myStates[ePlayerState::PRIMARY]->AddTransition(ePlayerState::WALK);
	myStates[ePlayerState::PRIMARY]->AddTransition(ePlayerState::DIE);

	myStates[ePlayerState::ARROW]->AddTransition(ePlayerState::IDLE);
	myStates[ePlayerState::ARROW]->AddTransition(ePlayerState::WALK);
	myStates[ePlayerState::ARROW]->AddTransition(ePlayerState::DIE);

	myStates[ePlayerState::SWIPE]->AddTransition(ePlayerState::IDLE);
	myStates[ePlayerState::SWIPE]->AddTransition(ePlayerState::WALK);
	myStates[ePlayerState::SWIPE]->AddTransition(ePlayerState::DIE);

	myStates[ePlayerState::DASH]->AddTransition(ePlayerState::IDLE);
	myStates[ePlayerState::DASH]->AddTransition(ePlayerState::WALK);
	myStates[ePlayerState::DASH]->AddTransition(ePlayerState::DIE);

	myStates[ePlayerState::FEAR]->AddTransition(ePlayerState::IDLE);
	myStates[ePlayerState::FEAR]->AddTransition(ePlayerState::WALK);
	myStates[ePlayerState::FEAR]->AddTransition(ePlayerState::DIE);

	myStates[ePlayerState::BLOODLUST]->AddTransition(ePlayerState::IDLE);
	myStates[ePlayerState::BLOODLUST]->AddTransition(ePlayerState::WALK);
	myStates[ePlayerState::BLOODLUST]->AddTransition(ePlayerState::DIE);

	myActiveState = ePlayerState::IDLE;
}