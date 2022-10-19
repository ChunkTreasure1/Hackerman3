#pragma once
#include "UIElement.h"
#include "Volt/Rendering/Texture/Texture2D.h"
#include <Volt/Asset/AssetManager.h>
#include <Volt/Rendering/Renderer.h>


class UISprite : public UIElement
{
//FUNCTIONS
public:
	UISprite() = default;

	UISprite(const char* aSpritePath)
	{
		myUIType = eUIElementType::SPRITE;
		myTexture = Volt::AssetManager::GetAsset<Volt::Texture2D>(aSpritePath);
		SetTransform(gem::mat4{ 1.0f });
		SetSize({ (float)myTexture->GetWidth(), (float)myTexture->GetHeight() });
		myColor = gem::vec4{ 1.0f,1.0f,1.0f,1.0f };
	}

	UISprite(const char* aSpritePath, gem::mat4 aTransform, gem::vec4 aColor)
	{
		myUIType = eUIElementType::SPRITE;
		myTexture = Volt::AssetManager::GetAsset<Volt::Texture2D>(aSpritePath);
		SetTransform(gem::mat4{ 1.0f });
		SetSize({ (float)myTexture->GetWidth(), (float)myTexture->GetHeight() });
		myColor = gem::vec4{ 1.0f,1.0f,1.0f,1.0f };
	}
	~UISprite() = default;

	void OnRender()
	{
		Volt::Renderer::SubmitSprite(GetTexture(), GetTransform(), 0, GetColor());
	}


	Ref<Volt::Texture2D> GetTexture() { return myTexture; }
	void SetTexture(Ref<Volt::Texture2D> aTexture) { myTexture = aTexture; }

	gem::vec4& GetColor() { return myColor; }
	void SetColor(gem::vec4 aColor) { myColor = aColor; }

private:

//VARIABLES
public:
	//bool isEnabled = true;

private:
	Ref<Volt::Texture2D> myTexture;
	gem::vec4 myColor;

};