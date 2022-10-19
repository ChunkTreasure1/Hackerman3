#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <istream>
#include <vector>

#include "UISprite.h"
#include "UIButton.hpp"
#include "UIText.h"
#include "UIPopUp.h"
#include "UISlider.h"

#include <queue>

#include <Volt/Core/Base.h>

#include <yaml-cpp/yaml.h>

struct UILoadData
{
	Ref<UICanvas>& aCanvas;
	std::vector<UISprite>& aSpriteVec;
	std::vector<UIButton>& aButtonsVec; 
	std::vector<UIText>& aTextVec;
	std::vector<UIPopUp>& aPopUpVec;
	std::vector<UISlider>& aSliderVec;
};


inline bool LoadUI(const char* aPath, Ref<UICanvas>& aCanvas,std::vector<UISprite>& aSpriteVec, std::vector<UIButton>& aButtonsVec, std::vector<UIText>& aTextVec, std::vector<UIPopUp>& aPopUpVec, std::vector<UISlider>& aSliderVec)
{
	std::ifstream file(aPath);
	std::stringstream sstream;
	sstream << file.rdbuf();

	YAML::Node root = YAML::Load(sstream.str());
	YAML::Node Elements = root["UIElement"];

	//std::string aName = "";

	if (Elements)
	{
		for (auto element : Elements)
		{
			eUIElementType currentType = static_cast<eUIElementType>(element["UIType"].as<int>());
			if (currentType == eUIElementType::SPRITE)
			{
				std::string aSpritePath = element["SpritePath"].as<std::string>();
				UISprite aSprite(aSpritePath.c_str());
				aSprite.SetCanvas(aCanvas);
				std::vector<float> position = element["Position"].as<std::vector<float>>();
				aSprite.SetPosition({ position[0],position[1] });

				aSpriteVec.push_back(aSprite);
			}
			else if (currentType == eUIElementType::BUTTON)
			{
				std::string aSpritePath = element["SpritePath"].as<std::string>();
				UISprite aSprite(aSpritePath.c_str());
				aSprite.SetCanvas(aCanvas);
				std::vector<float> spritePosition = element["SpritePosition"].as<std::vector<float>>();
				aSprite.SetPosition({ spritePosition[0],spritePosition[1] });

				UIButton aButton(aSprite);
				aButton.SetCanvas(aCanvas);
				std::vector<float> buttonPosition = element["ButtonPosition"].as<std::vector<float>>();
				aButton.SetPosition({ buttonPosition[0], buttonPosition[1] });
				aButton.SetButtonFunctionName(element["ButtonFunction"].as<std::string>());

				std::vector<float> buttonSize = element["ButtonSize"].as<std::vector<float>>();
				aButton.SetSize({ buttonSize[0], buttonSize[1] });

				aButtonsVec.push_back(aButton);
			}
			else if (currentType == eUIElementType::TEXT)
			{
				UIText aText;
				aText.SetCanvas(aCanvas);
				aText.SetText(element["Text"].as<std::string>());
				aText.SetFont(element["Font"].as<std::string>());
				aText.SetTextWidth(element["MaxWidth"].as<float>());
				std::vector<float> textPosition = element["Position"].as<std::vector<float>>();
				aText.SetPosition({ textPosition[0],textPosition[1] });
				std::vector<float> textSize = element["Size"].as<std::vector<float>>();
				aText.SetSize({ textSize[0],textSize[1] });

				aTextVec.push_back(aText);
			}
			else if (currentType == eUIElementType::POPUP)
			{
				UIPopUp aPopUp;
				aPopUp.SetCanvas(aCanvas);
				std::vector<float> popUpPos = element["PopUpPosition"].as<std::vector<float>>();
				aPopUp.SetPosition({ popUpPos[0],popUpPos[1] });
				std::vector<float> popUpSize = element["PopUpSize"].as<std::vector<float>>();
				aPopUp.SetSize({ popUpSize[0], popUpSize[1] });

				bool hasSprites = element["Sprites"].IsDefined();
				if (hasSprites)
				{
					for (auto Sprite : element["Sprites"])
					{
						std::string aSpritePath = Sprite["SpritePath"].as<std::string>();
						UISprite aSprite(aSpritePath.c_str());
						aSprite.SetCanvas(aCanvas);
						std::vector<float> spritePosition = Sprite["SpritePosition"].as<std::vector<float>>();
						aSprite.SetPosition({ spritePosition[0],spritePosition[1] });
						aPopUp.SetSprite(aSprite);
					}
				}

				bool hasText = element["Texts"].IsDefined();

				if (hasText)
				{
					for (auto text : element["Texts"])
					{
						UIText aText;
						aText.SetCanvas(aCanvas);
						aText.SetText(text["Text"].as<std::string>());
						aText.SetFont(text["Font"].as<std::string>());
						aText.SetTextWidth(text["MaxWidth"].as<float>());
						std::vector<float> textPosition = text["TextPosition"].as<std::vector<float>>();
						aText.SetPosition({ textPosition[0],textPosition[1] });
						std::vector<float> textSize = text["TextSize"].as<std::vector<float>>();
						aText.SetSize({ textSize[0],textSize[1] });
						aPopUp.SetText(aText);
					}
				}
				aPopUpVec.push_back(aPopUp);
			}
			else if(currentType == eUIElementType::SLIDER)
			{
				UISlider slider;
				slider.SetCanvas(aCanvas);
				std::vector<float> sliderPosition = element["Position"].as<std::vector<float>>();
				slider.SetPosition({ sliderPosition[0],sliderPosition[1] });
				slider.myLength = element["Length"].as<float>();
				slider.myValue = element["StartValue"].as<float>();

				UIText aText;
				aText.SetCanvas(aCanvas);
				aText.SetText(element["Text"].as<std::string>());
				aText.SetFont(element["Font"].as<std::string>());
				aText.SetTextWidth(element["MaxWidth"].as<float>());
				std::vector<float> textPosition = element["TextPosition"].as<std::vector<float>>();
				aText.SetPosition({ textPosition[0],textPosition[1] });
				std::vector<float> textSize = element["TextSize"].as<std::vector<float>>();
				aText.SetSize({ textSize[0],textSize[1] });
				slider.myDebugText = aText;
				slider.ReciveChild(slider.myDebugText);

				std::string spritePath = element["SpritePath"].as<std::string>();
				UISprite sprite(spritePath.c_str());
				sprite.SetCanvas(aCanvas);
				std::vector<float> spritePosition = element["SpritePosition"].as<std::vector<float>>();
				sprite.SetPosition({ spritePosition[0],spritePosition[1] });
				slider.myButtonSprite = sprite;
				slider.ReciveChild(slider.myButtonSprite);

				aSliderVec.push_back(slider);
			}
		}
	}

	file.close();
	return true;
}

inline bool LoadDialogue(const char* aPath, std::queue<std::string>& aDialogueQueue)
{
	std::ifstream file(aPath);
	std::stringstream sstream;
	sstream << file.rdbuf();

	YAML::Node root = YAML::Load(sstream.str());
	YAML::Node SceneLines = root["Scene"];

	for (auto line : SceneLines)
	{
		aDialogueQueue.push(line["Text"].as<std::string>());
	}

	file.close();

	return true;
}