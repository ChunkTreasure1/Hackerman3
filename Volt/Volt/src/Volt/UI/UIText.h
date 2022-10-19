#pragma once
#include "UIElement.h"
#include <string>
#include <Volt/Asset/AssetManager.h>
#include <Volt/Asset/Text/Font.h>

class UIText :public  UIElement
{
public:
	UIText()
	{
		myUIType = eUIElementType::TEXT;
		myColor = gem::vec4{ 1.0f,1.0f,1.0f,1.0f };
	}

	UIText(std::string aText)
	{
		myUIType = eUIElementType::TEXT;
		myFont = Volt::AssetManager::GetAsset<Volt::Font>("Assets/UI/Font/diablo.ttf");
		myMaxWidth = 512;
		myColor = gem::vec4{ 1.0f,1.0f,1.0f,1.0f };
		myText = aText;
	}

	void OnRender()
	{
		Volt::Renderer::SubmitString(myText, myFont, GetTransform(), myMaxWidth, myColor);
	}
	std::string& GetText() { return myText; }
	void SetText(std::string aString) { myText = aString; }

	Ref<Volt::Font> GetFont() { return myFont; }
	void SetFont(std::string aPath) { myFont = Volt::AssetManager::GetAsset<Volt::Font>(aPath.c_str()); }

	gem::vec4& GetColor() { return myColor; }
	void SetColor(gem::vec4 aColor) { myColor = aColor; }

	void SetTextWidth(float aWidth) { myMaxWidth = aWidth; }
	float GetMaxWidth() { return myMaxWidth; }

private:



public:


private:
	std::string myText;
	Ref<Volt::Font> myFont;
	float myMaxWidth = 0.f;
	gem::vec4 myColor = { 1.f };

};