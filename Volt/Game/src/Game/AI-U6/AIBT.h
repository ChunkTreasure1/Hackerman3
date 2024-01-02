#pragma once
#include "BrainTree.h"
#include "Volt/Scene/Entity.h"

class AIBT
{
public:
	void Init(Volt::Entity aEntity);
	void Update();

protected:
	Volt::Entity myEntity;

private:
	BrainTree::BehaviorTree myTree;
	BrainTree::Blackboard::Ptr myBlackBoard;
};