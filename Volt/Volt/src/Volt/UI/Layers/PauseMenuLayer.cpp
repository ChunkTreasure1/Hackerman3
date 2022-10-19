#include "vtpch.h"
#include "PauseMenuLayer.h"

#include <Volt/Core/Base.h>
#include <Volt/Core/Application.h>

#include <Volt/Input/KeyCodes.h>
#include <Volt/Input/MouseButtonCodes.h>
#include <Volt/Input/Input.h>

#include "Volt/UI/UILoader.h"
#include "Volt/UI/UIFunctionRegistry.h"

#include <iostream>

PauseMenuLayer::PauseMenuLayer(Ref<Volt::SceneRenderer>& aSceneRenderer) : UIBaseLayer(aSceneRenderer)
{
	mySettingPath = "Assets/UI/PauseMenu/PauseMenuLayout.yaml";
	LoadUI(mySettingPath.c_str(), myCanvas, mySprites, myButtons, myTexts, myPopups, mySliders);

	std::function<void()> EnablePauseMenuFunc = [this]()
	{
		this->OnResumeGame();
		this->Enable();
	};

	Volt::UIFunctionRegistry::AddFunc("EnablePause", EnablePauseMenuFunc);


	std::function<void()> DisablePauseMenuFunc = [this]() 
	{
		this->Disable();
	};

	Volt::UIFunctionRegistry::AddFunc("DisablePause", DisablePauseMenuFunc);

}

bool PauseMenuLayer::OnKeyEvent(Volt::KeyPressedEvent& e)
{
	if (e.GetKeyCode() == VT_KEY_ESCAPE)
	{
		if (myPauseUIisOpen) { myPauseUIisOpen = false; }
		else { myPauseUIisOpen = true; }
	}
	return false;
}

bool PauseMenuLayer::OnRender(Volt::AppRenderEvent& e)
{
	if (!myPauseUIisOpen) { return false; }

	Volt::Renderer::Begin();
	Volt::Renderer::BeginPass(renderPass, camera, false);

	Volt::Renderer::SetDepthState(Volt::DepthState::None);

	for (auto Sprite : mySprites)
	{
		Sprite.OnRender();
	}

	for (auto Button : myButtons)
	{
		Button.OnRender();
	}

	for (auto Slider : mySliders)
	{
		Slider.OnRender();
	}

	for (auto Text : myTexts)
	{
		Text.OnRender();
	}

	for (auto PopUp : myPopups)
	{
		PopUp.OnRender();
	}

	Volt::Renderer::DispatchSpritesWithShader(screenspaceShader);
	Volt::Renderer::DispatchText();

	Volt::Renderer::EndPass();
	Volt::Renderer::End();

	return false;
}

bool PauseMenuLayer::OnUpdate(Volt::AppUpdateEvent& e)
{
	if (!myPauseUIisOpen) { isMousePressed = false; return false; }

	renderPass.framebuffer = myRenderpassRef->GetFinalFramebuffer();

	std::pair mousePos = Volt::Input::GetMousePosition();

	//Convert MP to 0,0 center 
	gem::vec2 mouseViewPortPos = UIMath::ConvertToViewSpacePos({ mousePos.first, mousePos.second }, *myCanvas);
	gem::vec2 convMousePos = UIMath::ConvertPositionToCenter({ mousePos.first, mousePos.second }, *myCanvas);

	for (auto& button : myButtons)
	{
		if (button.IsInside(convMousePos))
		{
			//std::cout << "INSIDE" << std::endl;
			if (isMousePressed)
			{
				button.OnButtonPressed();
			}
		}
		else
		{
			//std::cout << "OUTSIDE" << std::endl;
		}
	}

	for (auto slider : mySliders)
	{
		slider.OnUpdate(convMousePos, isMousePressed);
	}

	for (auto& aPopUp : myPopups)
	{
		aPopUp.OnUpdate(convMousePos);
	}

	isMousePressed = false;

	return false;
}

void PauseMenuLayer::OnResumeGame()
{
	myPauseUIisOpen = false;
}



