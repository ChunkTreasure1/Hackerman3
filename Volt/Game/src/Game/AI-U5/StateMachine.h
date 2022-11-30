#pragma once

#include "Volt/Core/Base.h"
#include <functional>

class State
{
public:
	std::function<void()> onEnter;
	std::function<void()> onExit;
	std::function<void(float)> onUpdate;
	std::vector<std::function<int32_t()>> transitions;

private:
	friend class StateMachine;

	StateMachine* stateMachine;
};

class StateMachine
{
public:
	State& AddState();
	void SetStartState(uint32_t index);

	void Update(float aDeltaTime);

private:
	std::vector<Scope<State>> myStates;
	State* myCurrentState = nullptr;
};