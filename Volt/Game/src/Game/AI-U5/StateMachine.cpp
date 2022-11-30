#include "StateMachine.h"

State& StateMachine::AddState()
{
	Scope<State> state = CreateScope<State>();
	state->stateMachine = this;

	myStates.emplace_back(std::move(state));

	return *myStates.back();
}

void StateMachine::SetStartState(uint32_t index)
{
	myCurrentState = myStates.at(index).get();
}

void StateMachine::Update(float aDeltaTime)
{
	if (myCurrentState->onUpdate)
	{
		myCurrentState->onUpdate(aDeltaTime);
	}

	for (const auto& trans : myCurrentState->transitions)
	{
		int32_t result = trans();
		if (result != -1)
		{
			if (myCurrentState->onExit)
			{
				myCurrentState->onExit();
			}

			myCurrentState = myStates.at(result).get();
			if (myCurrentState->onEnter)
			{
				myCurrentState->onEnter();
			}

			break;
		}
	}
}
