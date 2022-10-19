#pragma once
#include <Volt/Core/Base.h>
#include <string>
#include <unordered_map>
#include <Volt/Scene/Entity.h>

template <typename StateType>
class StateBase
{
public:
	StateBase(const Volt::Entity& aEntity) { myEntity = aEntity; }
	virtual void OnExit() = 0;
	virtual void OnEnter() = 0;
	virtual void OnReset() = 0;
	virtual void OnUpdate(const float& deltaTime) = 0;
	virtual void OnFixedUpdate() = 0;

	bool DoTransition(StateType& aStateToChangeTo);
	bool AddTransition(StateType aTransition);
	bool RemoveTransition(StateType aTransition);

	bool SetTransition(StateType aTransition);

	std::unordered_map<StateType, bool>& GetTransitions() { return myTransitions; }
	Volt::Entity GetEntity() { return myEntity; }

protected:
	std::unordered_map<StateType, bool> myTransitions;
	Volt::Entity myEntity;
};

template <typename StateType>
bool StateBase<StateType>::DoTransition(StateType& aStateToChangeTo)
{
	for (auto it = myTransitions.begin(); it != myTransitions.end(); ++it)
		if (it->second == true)
		{
			aStateToChangeTo = it->first;
			it->second = false;
			return true;

		}

	return false;
}

template <typename StateType>
bool StateBase<StateType>::AddTransition(StateType aTransition)
{
	if (myTransitions.find(aTransition) != myTransitions.end())
		return false;
	std::pair<StateType, bool> transition{ aTransition,false };
	myTransitions.insert(transition);
	return true;
}

template <typename StateType>
bool StateBase<StateType>::RemoveTransition(StateType aTransition)
{
	auto it = myTransitions.find(aTransition);
	if (it == myTransitions.end())
		return false;
	myTransitions.erase(it);
	return true;
}

template<typename StateType>
inline bool StateBase<StateType>::SetTransition(StateType aTransition)
{
	auto it = myTransitions.find(aTransition);
	if (it == myTransitions.end())
		return false;
	it->second = true;
	return true;
}
