#pragma once
#include <Volt/Utility/Math.h>
#include "Volt/UI/UISprite.h"
#include "UIAABB.h"

#include <Volt/Rendering/Renderer.h>
#include "Volt/UI/UIFunctionRegistry.h"

class UIButton : public UIElement
{
	//FUNCTIONS
public:
	UIButton() : mySprite(UISprite("Assets/UI/spriteTest.dds"))
	{
		myUIType = eUIElementType::BUTTON;
		SetSize(mySprite.GetSize());
		SetPivot({(GetSize().x / 2), (GetSize().y / 2) });
		ReciveChild(mySprite);
	}

	UIButton(const char* aSpritePath) : mySprite(UISprite(aSpritePath))
	{
		myUIType = eUIElementType::BUTTON;
		SetSize(mySprite.GetSize());
		ReciveChild(mySprite);
	}

	UIButton(UISprite aSprite) : mySprite(aSprite)
	{
		myUIType = eUIElementType::BUTTON;
		SetSize(mySprite.GetSize());
		SetPivot({ (GetSize().x / 2), (GetSize().y / 2) });
		ReciveChild(mySprite);
	}

	~UIButton() = default;

	void OnRender()
	{
		Volt::Renderer::SubmitSprite(GetSprite().GetTexture(), GetSprite().GetTransform(), 0, GetSprite().GetColor());
	}

	UISprite& GetSprite() { return mySprite; }
	void SetSprite(UISprite& aSprite) { mySprite = aSprite; }

	const std::string& GetButtonFunctionName() { return myButtonFunction; }
	void SetButtonFunctionName(std::string aFunctionName) { myButtonFunction = aFunctionName; }

	bool IsInside(gem::vec2 mousePos)
	{
		return myCollider.IsInside(mousePos, *this);
	}

	virtual bool OnButtonPressed()
	{
		if (myButtonFunction == "") { return false; }
		return Volt::UIFunctionRegistry::Execute(myButtonFunction);
	}

private:

	//VARIABLES
public:

private:
	UISprite mySprite;
	//UISprite mySelectedSprite;
	
	std::string myButtonFunction;

	UIAABB myCollider;
};