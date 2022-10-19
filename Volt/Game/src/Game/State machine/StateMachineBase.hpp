#pragma once
#include <Volt/Core/Base.h>
#include <unordered_map>
#include <memory>
#include "StateBase.hpp"

template <typename StateType>
class StateMachineBase
{
public:
	StateMachineBase(const Volt::Entity& aEntity) { myEntity = aEntity; }
	virtual void CreateStates() = 0;

	void Update(const float& deltaTime);
	void FixedUpdate();
	void FullReset();
	void SetState(StateType newState, bool callExit = true, bool callEnter = true);

	StateType GetActiveState() { return myActiveState; }
	std::unordered_map<StateType, Ref<StateBase<StateType>>>& GetStates() { return myStates; }
	Volt::Entity GetEntity() { return myEntity; }

protected:
	StateType myActiveState;
	std::unordered_map<StateType, Ref<StateBase<StateType>>> myStates;
	Volt::Entity myEntity;
};


template <typename StateType>
void StateMachineBase<StateType>::FullReset()
{
	for (Ref<StateBase<StateType>>& ptr : myStates)
		ptr->OnReset();
}

template <typename StateType>
void StateMachineBase<StateType>::FixedUpdate()
{
	myStates[myActiveState]->OnFixedUpdate();
}

template <typename StateType>
void StateMachineBase<StateType>::Update(const float& deltaTime)
{
	myStates[myActiveState]->OnUpdate(deltaTime);
	StateType newState = static_cast<StateType>(-1);
	if (!myStates[myActiveState]->DoTransition(newState))
		return;
	myStates[myActiveState]->OnExit();
	myActiveState = newState;
	myStates[myActiveState]->OnEnter();
}

template <typename StateType>
void StateMachineBase<StateType>::SetState(StateType newState, bool callExit, bool callEnter)
{
	if (callExit)
		myStates[myActiveState]->OnExit();
	myActiveState = newState;
	if (callEnter)
		myStates[myActiveState]->OnEnter();
}