#include "AIBT.h"
#include "BTNodes.hpp"

void AIBT::Init(Volt::Entity aEntity)
{
	myEntity = aEntity;

	myBlackBoard = std::make_shared<BrainTree::Blackboard>();

	myBlackBoard->setEntity(myEntity);

	//myTree = BrainTree::Builder()
	//	.composite<BrainTree::Selector>()
	//		.decorator<LowHealth>(myBlackBoard)
	//			.leaf<LowHealthMove>(myBlackBoard)
	//		.end()
	//		.leaf<Reloading>(myBlackBoard)
	//		.decorator<SeePlayer>(myBlackBoard)
	//			.leaf<Shoot>(myBlackBoard)
	//		.end()
	//		.leaf<Wander>(myBlackBoard)
	//	.end()
	//	.build();

	myTree = BrainTree::Builder()
	.composite<BrainTree::Selector>()
		.composite<BrainTree::Sequence>()
			.leaf<IsDead>(myBlackBoard)
			.leaf<Respawn>(myBlackBoard)
		.end()
		.composite<BrainTree::Selector>()
			.composite<BrainTree::Sequence>()
				.leaf<LowHealth>(myBlackBoard)
				.decorator<IsOnHealthWell>(myBlackBoard)
					.leaf<GoToHealthWell>(myBlackBoard)
				.end()
			.end()
		.composite<BrainTree::Selector>()
			.composite<BrainTree::Sequence>()
				.decorator<SeePlayer>(myBlackBoard)
					.leaf<Shoot>(myBlackBoard)
				.end()
			.end()
		.leaf<Wander>(myBlackBoard)
		.end()
		.end()
		.end()
		.build();

}

void AIBT::Update()
{
	myTree.update();
}

