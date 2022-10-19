#include "vtpch.h"
#include "UIBaseLayer.h"

#include <Volt/Input/Input.h>
#include <Volt/UI/UIMath.h>
#include "Volt/UI/UILoader.h"

#include <Volt/Rendering/Shader/ShaderRegistry.h>

#include <Volt/Input/MouseButtonCodes.h>
#include <Volt/Input/KeyCodes.h>

#include <Volt/Rendering/Camera/Camera.h>

UIBaseLayer::UIBaseLayer(Ref<Volt::SceneRenderer>& aSceneRenderer) : myRenderpassRef(aSceneRenderer)
{
	myCanvas = std::make_shared<UICanvas>(1920.f, 1080.f);

	//Ortocamera
	camera = CreateRef<Volt::Camera>(myCanvas->left, myCanvas->right, myCanvas->bottom, myCanvas->top, 0.001f, 10000.f);
	screenspaceShader = Volt::ShaderRegistry::Get("Quad");
	
	//myTestVideo = Volt::AssetManager::GetAsset<Volt::Video>("Assets/Cutscenes/cutsceneIntroTest.mp4");
	//myTestVideo->Play(true);
}

void UIBaseLayer::Enable()
{
	isEnabled = true;
	isMousePressed = false;
}

void UIBaseLayer::Disable()
{
	isEnabled = false;
	isMousePressed = false;
}

void UIBaseLayer::OnAttach()
{
	renderPass.framebuffer = myRenderpassRef->GetFinalFramebuffer();
}

void UIBaseLayer::OnDetach()
{

}

void UIBaseLayer::OnEvent(Volt::Event& e)
{
	Volt::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Volt::OnSceneStopEvent>(VT_BIND_EVENT_FN(UIBaseLayer::OnStop));
	dispatcher.Dispatch<Volt::KeyPressedEvent>(VT_BIND_EVENT_FN(UIBaseLayer::OnKeyEvent));
	dispatcher.Dispatch<Volt::MouseButtonPressedEvent>(VT_BIND_EVENT_FN(UIBaseLayer::OnMouseEvent));
	dispatcher.Dispatch<Volt::ViewportResizeEvent>(VT_BIND_EVENT_FN(UIBaseLayer::OnViewportResize));
	dispatcher.Dispatch<Volt::WindowResizeEvent>(VT_BIND_EVENT_FN(UIBaseLayer::OnWindowResize));

	if (!isEnabled) { return; }

	dispatcher.Dispatch<Volt::AppUpdateEvent>(VT_BIND_EVENT_FN(UIBaseLayer::OnUpdate));
	dispatcher.Dispatch<Volt::AppRenderEvent>(VT_BIND_EVENT_FN(UIBaseLayer::OnRender));

}

bool UIBaseLayer::OnRender(Volt::AppRenderEvent& e)
{
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

	//Volt::Renderer::SubmitSprite(myTestVideo->GetTexture(), gem::scale(gem::mat4(1.f), gem::vec3{ 300.f, 300.f, 1.f }));

	Volt::Renderer::DispatchSpritesWithShader(screenspaceShader);
	Volt::Renderer::DispatchText();

	Volt::Renderer::EndPass();
	Volt::Renderer::End();

	return false;
}

bool UIBaseLayer::OnUpdate(Volt::AppUpdateEvent& e)
{
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

	//myTestVideo->Update(e.GetTimestep());

	isMousePressed = false;

	return false;
}

bool UIBaseLayer::OnPlay(Volt::OnScenePlayEvent& e)
{
	mySprites.clear();
	myButtons.clear();
	myTexts.clear();
	myPopups.clear();
	mySliders.clear();

	LoadUI(mySettingPath.c_str(), myCanvas, mySprites, myButtons, myTexts, myPopups, mySliders);
	return false;
}

bool UIBaseLayer::OnStop(Volt::OnSceneStopEvent& e)
{
	isEnabled = false;
	return false;
}

bool UIBaseLayer::OnKeyEvent(Volt::KeyPressedEvent& e)
{
	if (e.GetKeyCode() == VT_KEY_F8)
	{
		if (isEnabled) { isEnabled = false; }
		else { isEnabled = true; }
	}

	return false;
}

bool UIBaseLayer::OnMouseEvent(Volt::MouseButtonPressedEvent& e)
{
	if (e.GetMouseButton() == VT_MOUSE_BUTTON_LEFT)
	{
		isMousePressed = true;
	}

	return false;
}

bool UIBaseLayer::OnViewportResize(Volt::ViewportResizeEvent& e)
{
	float newHight = (float)e.GetHeight();
	float newWidth = (float)e.GetWidth();

	float viewportX = (float)e.GetX();
	float viewportY = (float)e.GetY() - 20.f;

	*myCanvas = UICanvas(newWidth, newHight, viewportX, viewportY);
	camera = CreateRef<Volt::Camera>(myCanvas->left, myCanvas->right, myCanvas->bottom, myCanvas->top, 0.001f, 10000.f);

	//Resize everything!!! :(
	mySprites.clear();
	myButtons.clear();
	myTexts.clear();
	myPopups.clear();
	mySliders.clear();

	LoadUI(mySettingPath.c_str(), myCanvas, mySprites, myButtons, myTexts, myPopups,mySliders);

	return false;
}

bool UIBaseLayer::OnWindowResize(Volt::WindowResizeEvent& e)
{
	float newHight = (float)e.GetHeight();
	float newWidth = (float)e.GetWidth();

	*myCanvas = UICanvas(newWidth, newHight);
	camera = CreateRef<Volt::Camera>(myCanvas->left, myCanvas->right, myCanvas->bottom, myCanvas->top, 0.001f, 10000.f);

	mySprites.clear();
	myButtons.clear();
	myTexts.clear();
	myPopups.clear();
	mySliders.clear();

	LoadUI(mySettingPath.c_str(), myCanvas, mySprites, myButtons, myTexts, myPopups, mySliders);

	return false;
}

