#include "vtpch.h"
#include "DialogueLayer.h"

#include <Volt/Core/Base.h>

#include <Volt/Input/KeyCodes.h>
#include <Volt/Input/MouseButtonCodes.h>
#include <Volt/Input/Input.h>

#include "Volt/UI/UILoader.h"
#include "Volt/UI/UIFunctionRegistry.h"

#include <iostream>

DialogueLayer::DialogueLayer(Ref<Volt::SceneRenderer>& aSceneRenderer) : UIBaseLayer(aSceneRenderer)
{
	VT_ASSERT(!myInstance, "DialogueLayer already exists");
	myInstance = this;

	mySettingPath = "Assets/UI/Dialogue/DialogueLayout.yaml";
	LoadUI(mySettingPath.c_str(), myCanvas, mySprites, myButtons, myTexts, myPopups, mySliders);

	isEnabled = false;
}

DialogueLayer::~DialogueLayer()
{
	myInstance = nullptr;
}

void DialogueLayer::NextDialogue()
{
	if (myLoadedDialogue.size() <= 0) { return; }
	myTexts[0].SetText(myLoadedDialogue.front());
	myLoadedDialogue.pop();

	if (myLoadedDialogue.size() <= 0)
	{
		myTexts[0].SetText("");
		isEnabled = false;
	}

}

bool DialogueLayer::OnKeyEvent(Volt::KeyPressedEvent& e)
{
	if (e.GetKeyCode() == VT_KEY_F8)
	{
		if (isEnabled) { isEnabled = false; }
		else { isEnabled = true; }
	}

	if (e.GetKeyCode() == VT_KEY_SPACE)
	{
		NextDialogue();
	}

	return false;
}

bool DialogueLayer::OnMouseEvent(Volt::MouseButtonPressedEvent& e)
{
	if (e.GetMouseButton() == VT_MOUSE_BUTTON_LEFT)
	{
		NextDialogue();
	}
	return false;
};

bool DialogueLayer::OnRender(Volt::AppRenderEvent& e)
{
	if (myLoadedDialogue.empty()) { return false; }

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

void DialogueLayer::LoadScene(std::string aPath)
{
	LoadDialogue(aPath.c_str(), myLoadedDialogue);
	isEnabled = true;
}
