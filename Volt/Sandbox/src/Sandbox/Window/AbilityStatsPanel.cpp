#include "sbpch.h"
#include "AbilityStatsPanel.h"
#include <yaml-cpp/yaml.h>
#include "Volt/Utility/SerializationMacros.h"
#include "Volt/Utility/YAMLSerializationHelpers.h"
#include "Volt/Utility/UIUtility.h"

AbilityStatsPanel::AbilityStatsPanel()
	: EditorWindow("Ability Editor"){
	InitNames();
}

void AbilityStatsPanel::UpdateMainContent(){
	static int currentAbilitySelected = 0;
	const char* abilityComboLable = myAbilityNames[currentAbilitySelected].c_str();
	if(ImGui::BeginCombo("Abilities", abilityComboLable)){
		for(int n = 0; n<myAbilityNames.size(); n++){
			const bool isSelected = ( currentAbilitySelected==n );
			if(ImGui::Selectable(myAbilityNames[n].c_str(), isSelected)){
				currentAbilitySelected = n;
			}

			if(isSelected){
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}

void AbilityStatsPanel::InitNames(){
	myAbilityNames =
	{
		"None",
		"Primary",
		"Arrow",
		"Swipe",
		"Dash",
		"Fear",
		"BloodLust",
		"LineSlam",
		"Knockback",
		"GasShield",
		"GasAttack",
		"RangedAttack",
		"StunAttack",
		"CloseAttack",
		"ChampionAttack",
	};
}
