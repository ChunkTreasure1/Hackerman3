#include "Game.h"

#include "AI/AIEventManager.h"
#include "AI/PollingStation.h"

#include <Volt/Core/Base.h>
#include <Volt/Log/Log.h>

Game::Game(Volt::Scene* scene)
	: myScene(scene)
{}

void Game::OnStart()
{
	myAIEventManager = CreateRef<AIEventManager>();
	myPollingStation = CreateRef<PollingStation>(myScene);
}

void Game::OnStop()
{
	myAIEventManager = nullptr;
	myPollingStation = nullptr;
	myScene = nullptr;
}

void Game::OnUpdate()
{
	myPollingStation->Update(0.f);
}