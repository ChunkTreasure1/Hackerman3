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


	if(ImGui::Button("Save Ability Data")){
		AbilityData::SaveData();
	}

	mySelectedAbility = static_cast<eAbilityList>( currentAbilitySelected );
	switch(mySelectedAbility){
	case eAbilityList::Primary:
		UI::PushId();
		if(UI::BeginProperties()){
			UI::Property("Damage", AbilityData::PrimaryData->damage);
			UI::Property("Cooldown", AbilityData::PrimaryData->cooldown);
			UI::Property("Size", AbilityData::PrimaryData->size);
			UI::Property("Fury Increase", AbilityData::PrimaryData->furyIncrease);
			UI::Property("Description", AbilityData::PrimaryData->description);
			UI::EndProperties();
		}
		UI::PopId();
		break;
	case eAbilityList::Arrow:
		UI::PushId();
		if(UI::BeginProperties()){
			UI::Property("Damage", AbilityData::ArrowData->damage);
			UI::Property("Cooldown", AbilityData::ArrowData->cooldown);
			UI::Property("LifeTime", AbilityData::ArrowData->lifetime);
			UI::Property("Speed", AbilityData::ArrowData->speed);
			UI::Property("Fury Increase", AbilityData::ArrowData->furyIncrease);
			UI::Property("Description", AbilityData::ArrowData->description);
			UI::EndProperties();
		}
		UI::PopId();
		break;
	case eAbilityList::Swipe:
		UI::PushId();
		if(UI::BeginProperties()){
			UI::Property("Damage", AbilityData::SwipeData->damage);
			UI::Property("Cooldown", AbilityData::SwipeData->cooldown);
			UI::Property("Size", AbilityData::SwipeData->size);
			UI::Property("Swipe Move Speed", AbilityData::SwipeData->swipeMoveSpeed);
			UI::Property("Cost", AbilityData::SwipeData->cost);
			UI::Property("Description", AbilityData::SwipeData->description);
			UI::EndProperties();
		}
		UI::PopId();
		break;
	case eAbilityList::Dash:
		UI::PushId();
		if(UI::BeginProperties()){
			UI::Property("Damage", AbilityData::DashData->damage);
			UI::Property("Cooldown", AbilityData::DashData->cooldown);
			UI::Property("Range", AbilityData::DashData->range);
			UI::Property("Cost", AbilityData::DashData->cost);
			UI::Property("Description", AbilityData::DashData->description);
			UI::EndProperties();
		}
		UI::PopId();
		break;
	case eAbilityList::Fear:
		UI::PushId();
		if(UI::BeginProperties()){
			UI::Property("Damage", AbilityData::FearData->damage);
			UI::Property("Cooldown", AbilityData::FearData->cooldown);
			UI::Property("FearTime", AbilityData::FearData->fearTime);
			UI::Property("Radius", AbilityData::FearData->radius);
			UI::Property("Cost", AbilityData::FearData->cost);
			UI::Property("Description", AbilityData::FearData->description);
			UI::EndProperties();
		}
		UI::PopId();
		break;
	case eAbilityList::BloodLust:
		UI::PushId();
		if(UI::BeginProperties())
		{
			UI::Property("Cooldown", AbilityData::BloodLustData->cooldown);
			UI::Property("Bloodlust Time", AbilityData::BloodLustData->buffTime);
			UI::Property("AbilityCooldown", AbilityData::BloodLustData->abilityCooldown);
			UI::Property("Cost", AbilityData::BloodLustData->cost);
			UI::Property("Description", AbilityData::BloodLustData->description);
			UI::EndProperties();
		}
		UI::PopId();
		break;
	case eAbilityList::CloseAttack:
		UI::PushId();
		if(UI::BeginProperties()){
			UI::Property("Cooldown", AbilityData::CloseAttackData->cooldown);
			UI::Property("TimeAlive", AbilityData::CloseAttackData->timeAlive);
			UI::Property("Damage", AbilityData::CloseAttackData->damage);
			UI::Property("Size", AbilityData::CloseAttackData->size);
			UI::Property("Description", AbilityData::CloseAttackData->description);
			UI::EndProperties();
		}
		UI::PopId();
		break;
	case eAbilityList::ChampionAttack:
		UI::PushId();
		if(UI::BeginProperties()){
			UI::Property("Cooldown", AbilityData::ChampionAttackData->cooldown);
			UI::Property("Duration", AbilityData::ChampionAttackData->duration);
			UI::Property("Description", AbilityData::ChampionAttackData->description);
			UI::EndProperties();
		}
		UI::PopId();
		break;
	case eAbilityList::StunAttack:
		UI::PushId();
		if(UI::BeginProperties()){
			UI::Property("Cooldown", AbilityData::StunAttackData->cooldown);
			UI::Property("Duration", AbilityData::StunAttackData->duration);
			UI::Property("Time Alive", AbilityData::StunAttackData->timeAlive);
			UI::Property("Damage", AbilityData::StunAttackData->damage);
			UI::Property("Size", AbilityData::StunAttackData->size);
			UI::Property("Description", AbilityData::StunAttackData->description);
			UI::EndProperties();
		}
		UI::PopId();
		break;
	case eAbilityList::RangedAttack:
		UI::PushId();
		if(UI::BeginProperties()){
			UI::Property("Cooldown", AbilityData::RangedAttackData->cooldown);
			UI::Property("Speed", AbilityData::RangedAttackData->speed);
			UI::Property("Range", AbilityData::RangedAttackData->range);
			UI::Property("Damage", AbilityData::RangedAttackData->damage);
			UI::Property("Size", AbilityData::RangedAttackData->size);
			UI::Property("Description", AbilityData::RangedAttackData->description);
			UI::EndProperties();
		}
		UI::PopId();
		break;
	case eAbilityList::GasAttack:
		UI::PushId();
		if(UI::BeginProperties()){
			UI::Property("Cooldown", AbilityData::GasAttackData->cooldown);
			UI::Property("Radius", AbilityData::GasAttackData->radius);
			UI::Property("Description", AbilityData::GasAttackData->description);
			UI::EndProperties();
		}
		UI::PopId();
		break;
	case eAbilityList::GasShield:
		UI::PushId();
		if(UI::BeginProperties()){
			UI::Property("Radius", AbilityData::GasShieldData->radius);
			UI::Property("Description", AbilityData::GasShieldData->description);
			UI::EndProperties();
		}
		UI::PopId();
		break;
	case eAbilityList::Knockback:
		UI::PushId();
		if(UI::BeginProperties()){
			UI::Property("Cooldown", AbilityData::KnockbackData->cooldown);
			UI::Property("Force", AbilityData::KnockbackData->force);
			UI::Property("StunDuration", AbilityData::KnockbackData->stunDuration);
			UI::Property("Size", AbilityData::KnockbackData->size);
			UI::Property("Description", AbilityData::KnockbackData->description);
			UI::EndProperties();
		}
		UI::PopId();
		break;
	case eAbilityList::LineSlam:
		UI::PushId();
		if(UI::BeginProperties()){
			UI::Property("Cooldown", AbilityData::LineSlamData->cooldown);
			UI::Property("Collider Lifetime", AbilityData::LineSlamData->colliderLifetime);
			UI::Property("Size", AbilityData::LineSlamData->size);
			UI::Property("Description", AbilityData::LineSlamData->description);
			UI::EndProperties();
		}
		UI::PopId();
		break;

	case eAbilityList::None:
		break;
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
