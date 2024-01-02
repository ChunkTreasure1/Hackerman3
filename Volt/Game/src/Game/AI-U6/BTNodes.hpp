#pragma once
#include "BrainTree.h"
#include "AIBehaviourTreeScript.h"

class LowHealth : public BrainTree::Leaf
{
public:
	using BrainTree::Leaf::Leaf;

	Status update() override
	{
		auto script = blackboard->getEntity().GetScript<AIBehaviourTreeScript>("AIBehaviourTreeScript");
		if (script->LowHealth())
		{
			return Status::Success;
		}
		return Status::Failure;
	}
private:
};

class IsOnHealthWell : public BrainTree::Decorator
{
public:
	using BrainTree::Decorator::Decorator;

	Status update() override
	{
		auto script = blackboard->getEntity().GetScript<AIBehaviourTreeScript>("AIBehaviourTreeScript");
		if (!script->IsOnHealthWell())
		{
			child->update();
			return Status::Success;
		}
		return Status::Failure;
	}
private:
};


class GoToHealthWell :public BrainTree::Leaf
{
public:
	using BrainTree::Leaf::Leaf;

	Status update() override
	{
		auto script = blackboard->getEntity().GetScript<AIBehaviourTreeScript>("AIBehaviourTreeScript");
		script->GoToHealthWell();
		return Status::Success;
	}
private:
};

class IsDead :public BrainTree::Leaf
{
public:
	using BrainTree::Leaf::Leaf;

	Status update() override
	{
		auto script = blackboard->getEntity().GetScript<AIBehaviourTreeScript>("AIBehaviourTreeScript");
		if (script->IsDead())
		{
			return Status::Success;
		}
		return Status::Failure;
	}
private:
};

class Respawn : public BrainTree::Leaf
{
public:
	using BrainTree::Leaf::Leaf;

	Status update() override
	{
		auto script = blackboard->getEntity().GetScript<AIBehaviourTreeScript>("AIBehaviourTreeScript");
		if (script->UpdateRespawn())
		{
			return Status::Running;
		}
		return Status::Failure;
	}
private:
};

class SeePlayer : public BrainTree::Decorator
{
public:
	using BrainTree::Decorator::Decorator;

	Status update() override
	{
		auto script = blackboard->getEntity().GetScript<AIBehaviourTreeScript>("AIBehaviourTreeScript");
		if (script->CanSeeEnemy())
		{

			child->update();
			return Status::Success;
		}
		return Status::Failure;
	}
private:
};

class Wander : public BrainTree::Leaf
{
public:
	using BrainTree::Leaf::Leaf;

	Status update() override
	{
		auto script = blackboard->getEntity().GetScript<AIBehaviourTreeScript>("AIBehaviourTreeScript");
		script->Wander();
		return Status::Success;
	}

private:
};

class Shoot : public BrainTree::Leaf
{
public:
	using BrainTree::Leaf::Leaf;

	Status update() override
	{
		auto script = blackboard->getEntity().GetScript<AIBehaviourTreeScript>("AIBehaviourTreeScript");
		if (script->Shoot())
		{
			return Status::Success;
		}
		return Status::Failure;
	}

private:

};
