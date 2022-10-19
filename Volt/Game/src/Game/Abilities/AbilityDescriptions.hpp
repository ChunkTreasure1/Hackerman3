#pragma once
#include <Volt/Core/Base.h>
#include "GEM/gem.h"
#include <yaml-cpp/yaml.h>
#include "Volt/Utility/SerializationMacros.h"
#include "Volt/Utility/YAMLSerializationHelpers.h"
#include "Volt/Utility/FileSystem.h"
#include <filesystem>
#include <fstream>

enum class eAbilityList
{
	None,
	Primary,
	Arrow,
	Swipe,
	Dash,
	Fear,
	BloodLust,

	LineSlam,
	Knockback,
	GasShield,
	GasAttack,

	RangedAttack,
	StunAttack,
	CloseAttack,
	ChampionAttack
};

class AbilityData
{
public:
	static void Init();
	static void Shutdown();

	static void SaveData();

	struct PrimaryAbility
	{
		float cooldown = 0;
		gem::vec3 size = { 0 };
		int damage = 0;
		int furyIncrease = 0;
		std::string description = "player primary";
	};
	inline static Ref<PrimaryAbility> PrimaryData;

	struct ArrowAbility
	{
		float cooldown = 0;
		float speed = 0;
		float lifetime = 0;
		int damage = 0;
		int furyIncrease = 0;
		std::string description = "player secondary";
	};
	inline static Ref<ArrowAbility> ArrowData;

	struct SwipeAbility
	{
		float cooldown = 0;
		gem::vec3 size = { 0 };
		int damage = 0;
		float swipeMoveSpeed = 0;
		int cost = 0;
		std::string description = "player aoe";
	};
	inline static Ref<SwipeAbility> SwipeData;

	struct DashAbility
	{
		float cooldown = 0;
		float range = 0;
		int damage = 0;
		int cost = 0;
		std::string description = "player movement";
	};
	inline static Ref<DashAbility> DashData;

	struct FearAbility
	{
		float cooldown = 0;
		float fearTime = 0;
		float radius = 0;
		int damage = 0;
		int cost = 0;
		std::string description = "player cc";
	};
	inline static Ref<FearAbility> FearData;

	struct BloodLustAbility
	{
		float cooldown = 0;
		float buffTime = 0;
		float abilityCooldown = 0;
		int cost = 0;
		std::string description = "player ult";
	};
	inline static Ref<BloodLustAbility> BloodLustData;

	struct CloseAttack
	{
		float cooldown = 0;
		float timeAlive = 0;
		int damage = 0;
		gem::vec3 size =  {0};
		std::string description = "Melee enemy attack";
	};

	inline static Ref<CloseAttack> CloseAttackData;
	// ???????????
	struct ChampionAttack
	{
		float cooldown = 0;
		float duration = 0;
		std::string description = "Champ themed attack thing";
	};
	inline static Ref<ChampionAttack> ChampionAttackData;

	struct StunAttack
	{
		float cooldown = 0;
		//if knock back as well?
		//float force;
		float duration = 0;
		float timeAlive = 0;
		float chargeUp = 0;
		int damage = 0;
		gem::vec3 size = {0};
		std::string description = "Bruiser stun attack";
	};
	inline static Ref<StunAttack> StunAttackData;


	struct RangedAttack
	{
		float cooldown = 0;
		float range = 0;
		float speed = 0;
		int damage = 0;
		gem::vec3 size = {0};
		std::string description = "Ranged enemy attack";
	};
	inline static Ref<RangedAttack> RangedAttackData;

	struct GasAttack
	{
		float cooldown = 0;
		// Maybe vec 3 if not circle
		float radius = 0;
		std::string description ="Boss gas attack phase 3";
	};
	inline static Ref<GasAttack> GasAttackData;

	struct GasShield
	{
		float radius = 0;
		std::string description = "Boss gas shield phase 2";
	};
	inline static Ref<GasShield> GasShieldData;

	struct Knockback
	{
		float cooldown = 0;
		float force = 0;
		float stunDuration = 0;
		// Maybe radius if circle
		gem::vec3 size = {0};
		std::string description = "Boss knockback when standing and hitting phase 1";
	};
	inline static Ref<Knockback> KnockbackData;

	struct LineSlam
	{
		float cooldown = 0;
		float colliderLifetime = 0;
		gem::vec3 size = {0};
		std::string description = "Boss line slam phase 1";
	};
	inline static Ref<LineSlam> LineSlamData;
};

inline void AbilityData::Init()
{
	PrimaryData = CreateRef<PrimaryAbility>();
	ArrowData = CreateRef<ArrowAbility>();
	SwipeData = CreateRef<SwipeAbility>();
	DashData = CreateRef<DashAbility>();
	FearData = CreateRef<FearAbility>();
	BloodLustData = CreateRef<BloodLustAbility>();

	CloseAttackData = CreateRef<CloseAttack>();
	ChampionAttackData = CreateRef<ChampionAttack>();
	StunAttackData = CreateRef<StunAttack>();
	RangedAttackData = CreateRef<RangedAttack>();
	GasAttackData = CreateRef<GasAttack>();
	GasShieldData = CreateRef<GasShield>();
	KnockbackData = CreateRef<Knockback>();
	LineSlamData = CreateRef<LineSlam>();

	std::ifstream file("Assets/Data/AbilityData.yaml");
	std::stringstream sstream;
	sstream << file.rdbuf();

	YAML::Node root = YAML::Load(sstream.str());
	YAML::Node abilitiesNode = root["Abilities"];
	if (abilitiesNode)
	{
		if (abilitiesNode["Primary"])
		{
			VT_DESERIALIZE_PROPERTY(cooldown, PrimaryData->cooldown, abilitiesNode["Primary"], 0.f);
			VT_DESERIALIZE_PROPERTY(size, PrimaryData->size, abilitiesNode["Primary"], gem::vec3(0.f));
			VT_DESERIALIZE_PROPERTY(damage, PrimaryData->damage, abilitiesNode["Primary"], 0);
			VT_DESERIALIZE_PROPERTY(furyIncrease, PrimaryData->furyIncrease, abilitiesNode["Primary"], 0);
			VT_DESERIALIZE_PROPERTY(description, PrimaryData->description, abilitiesNode["Primary"], std::string(""));
		}
		if (abilitiesNode["Arrow"])
		{
			VT_DESERIALIZE_PROPERTY(cooldown, ArrowData->cooldown, abilitiesNode["Arrow"], 0.f);
			VT_DESERIALIZE_PROPERTY(speed, ArrowData->speed, abilitiesNode["Arrow"], 0.f);
			VT_DESERIALIZE_PROPERTY(lifetime, ArrowData->lifetime, abilitiesNode["Arrow"], 0.f);
			VT_DESERIALIZE_PROPERTY(damage, ArrowData->damage, abilitiesNode["Arrow"], 0);
			VT_DESERIALIZE_PROPERTY(furyIncrease, ArrowData->furyIncrease, abilitiesNode["Arrow"], 0);
			VT_DESERIALIZE_PROPERTY(description, ArrowData->description, abilitiesNode["Arrow"], std::string(""));
		}
		if (abilitiesNode["Swipe"])
		{
			VT_DESERIALIZE_PROPERTY(cooldown, SwipeData->cooldown, abilitiesNode["Swipe"], 0.f);
			VT_DESERIALIZE_PROPERTY(size, SwipeData->size, abilitiesNode["Swipe"], gem::vec3(0.f));
			VT_DESERIALIZE_PROPERTY(damage, SwipeData->damage, abilitiesNode["Swipe"], 0);
			VT_DESERIALIZE_PROPERTY(swipeMoveSpeed, SwipeData->swipeMoveSpeed, abilitiesNode["Swipe"], 0.f);
			VT_DESERIALIZE_PROPERTY(cost, SwipeData->cost, abilitiesNode["Swipe"], 0);
			VT_DESERIALIZE_PROPERTY(description, SwipeData->description, abilitiesNode["Swipe"], std::string(""));
		}
		if (abilitiesNode["Dash"])
		{
			VT_DESERIALIZE_PROPERTY(cooldown, DashData->cooldown, abilitiesNode["Dash"], 0.f);
			VT_DESERIALIZE_PROPERTY(range, DashData->range, abilitiesNode["Dash"], 0.f);
			VT_DESERIALIZE_PROPERTY(damage, DashData->damage, abilitiesNode["Dash"], 0);
			VT_DESERIALIZE_PROPERTY(cost, DashData->cost, abilitiesNode["Dash"], 0);
			VT_DESERIALIZE_PROPERTY(description, DashData->description, abilitiesNode["Dash"], std::string(""));
		}
		if (abilitiesNode["Fear"])
		{
			VT_DESERIALIZE_PROPERTY(cooldown, FearData->cooldown, abilitiesNode["Fear"], 0.f);
			VT_DESERIALIZE_PROPERTY(fearTime, FearData->fearTime, abilitiesNode["Fear"], 0.f);
			VT_DESERIALIZE_PROPERTY(radius, FearData->radius, abilitiesNode["Fear"], 0.f);
			VT_DESERIALIZE_PROPERTY(damage, FearData->damage, abilitiesNode["Fear"], 0);
			VT_DESERIALIZE_PROPERTY(cost, FearData->cost, abilitiesNode["Fear"], 0);
			VT_DESERIALIZE_PROPERTY(description, FearData->description, abilitiesNode["Fear"], std::string(""));
		}
		if (abilitiesNode["BloodLust"])
		{
			VT_DESERIALIZE_PROPERTY(cooldown, BloodLustData->cooldown, abilitiesNode["BloodLust"], 0.f);
			VT_DESERIALIZE_PROPERTY(buffTime, BloodLustData->buffTime, abilitiesNode["BloodLust"], 0.f);
			VT_DESERIALIZE_PROPERTY(abilityCooldown, BloodLustData->abilityCooldown, abilitiesNode["BloodLust"], 0.f);
			VT_DESERIALIZE_PROPERTY(cost, BloodLustData->cost, abilitiesNode["BloodLust"], 0);
			VT_DESERIALIZE_PROPERTY(description, BloodLustData->description, abilitiesNode["BloodLust"], std::string(""));
		}
		if (abilitiesNode["CloseAttack"])
		{
			VT_DESERIALIZE_PROPERTY(cooldown, CloseAttackData->cooldown, abilitiesNode["CloseAttack"], 0.f);
			VT_DESERIALIZE_PROPERTY(timeAlive, CloseAttackData->timeAlive, abilitiesNode["CloseAttack"], 0.f);
			VT_DESERIALIZE_PROPERTY(damage, CloseAttackData->damage, abilitiesNode["CloseAttack"], 0);
			VT_DESERIALIZE_PROPERTY(size, CloseAttackData->size, abilitiesNode["CloseAttack"], gem::vec3(0.f));
			VT_DESERIALIZE_PROPERTY(description, CloseAttackData->description, abilitiesNode["CloseAttack"], std::string(""));
		}
		if (abilitiesNode["ChampionAttack"])
		{
			VT_DESERIALIZE_PROPERTY(cooldown, ChampionAttackData->cooldown, abilitiesNode["ChampionAttack"], 0.f);
			VT_DESERIALIZE_PROPERTY(duration, ChampionAttackData->duration, abilitiesNode["ChampionAttack"], 0.f);
			VT_DESERIALIZE_PROPERTY(description, ChampionAttackData->description, abilitiesNode["ChampionAttack"], std::string(""));
		}
		if (abilitiesNode["StunAttack"])
		{
			VT_DESERIALIZE_PROPERTY(cooldown, StunAttackData->cooldown, abilitiesNode["StunAttack"], 0.f);
			VT_DESERIALIZE_PROPERTY(duration, StunAttackData->duration, abilitiesNode["StunAttack"], 0.f);
			VT_DESERIALIZE_PROPERTY(timeAlive, StunAttackData->timeAlive, abilitiesNode["StunAttack"], 0.f);
			VT_DESERIALIZE_PROPERTY(damage, StunAttackData->damage, abilitiesNode["StunAttack"], 0);
			VT_DESERIALIZE_PROPERTY(size, StunAttackData->size, abilitiesNode["StunAttack"], gem::vec3(0.f));
			VT_DESERIALIZE_PROPERTY(description, StunAttackData->description, abilitiesNode["StunAttack"], std::string(""));
		}
		if (abilitiesNode["RangedAttack"])
		{
			VT_DESERIALIZE_PROPERTY(cooldown, RangedAttackData->cooldown, abilitiesNode["RangedAttack"], 0.f);
			VT_DESERIALIZE_PROPERTY(range, RangedAttackData->range, abilitiesNode["RangedAttack"], 0.f);
			VT_DESERIALIZE_PROPERTY(speed, RangedAttackData->speed, abilitiesNode["RangedAttack"], 0.f);
			VT_DESERIALIZE_PROPERTY(damage, RangedAttackData->damage, abilitiesNode["RangedAttack"], 0);
			VT_DESERIALIZE_PROPERTY(size, RangedAttackData->size, abilitiesNode["RangedAttack"], gem::vec3(0.f));
			VT_DESERIALIZE_PROPERTY(description, RangedAttackData->description, abilitiesNode["RangedAttack"], std::string(""));
		}
		if (abilitiesNode["GasAttack"])
		{
			VT_DESERIALIZE_PROPERTY(cooldown, GasAttackData->cooldown, abilitiesNode["GasAttack"], 0.f);
			VT_DESERIALIZE_PROPERTY(radius, GasAttackData->radius, abilitiesNode["GasAttack"], 0.f);
			VT_DESERIALIZE_PROPERTY(description, GasAttackData->description, abilitiesNode["GasAttack"], std::string(""));
		}
		if (abilitiesNode["GasShield"])
		{
			VT_DESERIALIZE_PROPERTY(radius, GasShieldData->radius, abilitiesNode["GasShield"], 0.f);
			VT_DESERIALIZE_PROPERTY(description, GasShieldData->description, abilitiesNode["GasShield"], std::string(""));
		}
		if (abilitiesNode["Knockback"])
		{
			VT_DESERIALIZE_PROPERTY(cooldown, KnockbackData->cooldown, abilitiesNode["Knockback"], 0.f);
			VT_DESERIALIZE_PROPERTY(force, KnockbackData->force, abilitiesNode["Knockback"], 0.f);
			VT_DESERIALIZE_PROPERTY(stunDuration, KnockbackData->stunDuration, abilitiesNode["Knockback"], 0.f);
			VT_DESERIALIZE_PROPERTY(size, KnockbackData->size, abilitiesNode["Knockback"], gem::vec3(0.f));
			VT_DESERIALIZE_PROPERTY(description, KnockbackData->description, abilitiesNode["Knockback"], std::string(""));
		}
		if (abilitiesNode["LineSlam"])
		{
			VT_DESERIALIZE_PROPERTY(cooldown, LineSlamData->cooldown, abilitiesNode["LineSlam"], 0.f);
			VT_DESERIALIZE_PROPERTY(colliderLifetime, LineSlamData->colliderLifetime, abilitiesNode["LineSlam"], 0.f);
			VT_DESERIALIZE_PROPERTY(size, LineSlamData->size, abilitiesNode["LineSlam"], gem::vec3(0.f));
			VT_DESERIALIZE_PROPERTY(description, LineSlamData->description, abilitiesNode["LineSlam"], std::string(""));
		}
	}
}

inline void AbilityData::SaveData()
{
	//std::filesystem::path path = FileSystem::SaveFile("Ability File(*.yaml)\0*.yaml\0");
	//if (path.empty()) 
	//{
	//	return;
	//}

	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Abilities" << YAML::Value;
	{
		out << YAML::BeginMap;
		out << YAML::Key << "Primary" << YAML::Value;
		{
			out << YAML::BeginMap;
			VT_SERIALIZE_PROPERTY(cooldown, PrimaryData->cooldown, out);
			VT_SERIALIZE_PROPERTY(size, PrimaryData->size, out);
			VT_SERIALIZE_PROPERTY(damage, PrimaryData->damage, out);
			VT_SERIALIZE_PROPERTY(furyIncrease, PrimaryData->furyIncrease, out);
			VT_SERIALIZE_PROPERTY(description, PrimaryData->description, out);
			out << YAML::EndMap;
		}

		out << YAML::Key << "Arrow" << YAML::Value;
		{
			out << YAML::BeginMap;
			VT_SERIALIZE_PROPERTY(cooldown, ArrowData->cooldown, out);
			VT_SERIALIZE_PROPERTY(damage, ArrowData->damage, out);
			VT_SERIALIZE_PROPERTY(lifetime, ArrowData->lifetime, out);
			VT_SERIALIZE_PROPERTY(speed, ArrowData->speed, out);
			VT_SERIALIZE_PROPERTY(furyIncrease, ArrowData->furyIncrease, out);
			VT_SERIALIZE_PROPERTY(description, ArrowData->description, out);
			out << YAML::EndMap;
		}

		out << YAML::Key << "Swipe" << YAML::Value;
		{
			out << YAML::BeginMap;
			VT_SERIALIZE_PROPERTY(cooldown, SwipeData->cooldown, out);
			VT_SERIALIZE_PROPERTY(size, SwipeData->size, out);
			VT_SERIALIZE_PROPERTY(damage, SwipeData->damage, out);
			VT_SERIALIZE_PROPERTY(swipeMoveSpeed, SwipeData->swipeMoveSpeed, out);
			VT_SERIALIZE_PROPERTY(cost, SwipeData->cost, out);
			VT_SERIALIZE_PROPERTY(description, SwipeData->description, out);
			out << YAML::EndMap;
		}

		out << YAML::Key << "Dash" << YAML::Value;
		{
			out << YAML::BeginMap;
			VT_SERIALIZE_PROPERTY(cooldown, DashData->cooldown, out);
			VT_SERIALIZE_PROPERTY(range, DashData->range, out);
			VT_SERIALIZE_PROPERTY(damage, DashData->damage, out);
			VT_SERIALIZE_PROPERTY(cost, DashData->cost, out);
			VT_SERIALIZE_PROPERTY(description, DashData->description, out);
			out << YAML::EndMap;
		}

		out << YAML::Key << "Fear" << YAML::Value;
		{
			out << YAML::BeginMap;
			VT_SERIALIZE_PROPERTY(cooldown, FearData->cooldown, out);
			VT_SERIALIZE_PROPERTY(fearTime, FearData->fearTime, out);
			VT_SERIALIZE_PROPERTY(damage, FearData->damage, out);
			VT_SERIALIZE_PROPERTY(radius, FearData->radius, out);
			VT_SERIALIZE_PROPERTY(cost, FearData->cost, out);
			VT_SERIALIZE_PROPERTY(description, FearData->description, out);
			out << YAML::EndMap;
		}

		out << YAML::Key << "BloodLust" << YAML::Value;
		{
			out << YAML::BeginMap;
			VT_SERIALIZE_PROPERTY(cooldown, BloodLustData->cooldown, out);
			VT_SERIALIZE_PROPERTY(buffTime, BloodLustData->buffTime, out);
			VT_SERIALIZE_PROPERTY(abilityCooldown, BloodLustData->abilityCooldown, out);
			VT_SERIALIZE_PROPERTY(cost, BloodLustData->cost, out);
			VT_SERIALIZE_PROPERTY(description, BloodLustData->description, out);
			out << YAML::EndMap;
		}

		out << YAML::Key << "CloseAttack" << YAML::Value;
		{
			out << YAML::BeginMap;
			VT_SERIALIZE_PROPERTY(cooldown, CloseAttackData->cooldown, out);
			VT_SERIALIZE_PROPERTY(timeAlive, CloseAttackData->timeAlive, out);
			VT_SERIALIZE_PROPERTY(damage, CloseAttackData->damage, out);
			VT_SERIALIZE_PROPERTY(size, CloseAttackData->size, out);
			VT_SERIALIZE_PROPERTY(description, CloseAttackData->description, out);
			out << YAML::EndMap;
		}

		out << YAML::Key << "ChampionAttack" << YAML::Value;
		{
			out << YAML::BeginMap;
			VT_SERIALIZE_PROPERTY(cooldown, ChampionAttackData->cooldown, out);
			VT_SERIALIZE_PROPERTY(duration, ChampionAttackData->duration, out);
			VT_SERIALIZE_PROPERTY(description, ChampionAttackData->description, out);
			out << YAML::EndMap;
		}

		out << YAML::Key << "StunAttack" << YAML::Value;
		{
			out << YAML::BeginMap;
			VT_SERIALIZE_PROPERTY(cooldown, StunAttackData->cooldown, out);
			VT_SERIALIZE_PROPERTY(duration, StunAttackData->duration, out);
			VT_SERIALIZE_PROPERTY(timeAlive, StunAttackData->timeAlive, out);
			VT_SERIALIZE_PROPERTY(damage, StunAttackData->damage, out);
			VT_SERIALIZE_PROPERTY(size, StunAttackData->size, out);
			VT_SERIALIZE_PROPERTY(description, StunAttackData->description, out);
			out << YAML::EndMap;
		}

		out << YAML::Key << "RangedAttack" << YAML::Value;
		{
			out << YAML::BeginMap;
			VT_SERIALIZE_PROPERTY(cooldown, RangedAttackData->cooldown, out);
			VT_SERIALIZE_PROPERTY(range, RangedAttackData->range, out);
			VT_SERIALIZE_PROPERTY(speed, RangedAttackData->speed, out);
			VT_SERIALIZE_PROPERTY(damage, RangedAttackData->damage, out);
			VT_SERIALIZE_PROPERTY(size, RangedAttackData->size, out);
			VT_SERIALIZE_PROPERTY(description, RangedAttackData->description, out);
			out << YAML::EndMap;
		}

		out << YAML::Key << "GasAttack" << YAML::Value;
		{
			out << YAML::BeginMap;
			VT_SERIALIZE_PROPERTY(cooldown, GasAttackData->cooldown, out);
			VT_SERIALIZE_PROPERTY(radius, GasAttackData->radius, out);
			VT_SERIALIZE_PROPERTY(description, GasAttackData->description, out);
			out << YAML::EndMap;
		}

		out << YAML::Key << "GasShield" << YAML::Value;
		{
			out << YAML::BeginMap;
			VT_SERIALIZE_PROPERTY(radius, GasShieldData->radius, out);
			VT_SERIALIZE_PROPERTY(description, GasShieldData->description, out);
			out << YAML::EndMap;
		}

		out << YAML::Key << "Knockback" << YAML::Value;
		{
			out << YAML::BeginMap;
			VT_SERIALIZE_PROPERTY(cooldown, KnockbackData->cooldown, out);
			VT_SERIALIZE_PROPERTY(force, KnockbackData->force, out);
			VT_SERIALIZE_PROPERTY(stunDuration, KnockbackData->stunDuration, out);
			VT_SERIALIZE_PROPERTY(size, KnockbackData->size, out);
			VT_SERIALIZE_PROPERTY(description, KnockbackData->description, out);
			out << YAML::EndMap;
		}

		out << YAML::Key << "LineSlam" << YAML::Value;
		{
			out << YAML::BeginMap;
			VT_SERIALIZE_PROPERTY(cooldown, LineSlamData->cooldown, out);
			VT_SERIALIZE_PROPERTY(colliderLifetime, LineSlamData->colliderLifetime, out);
			VT_SERIALIZE_PROPERTY(size, LineSlamData->size, out);
			VT_SERIALIZE_PROPERTY(description, LineSlamData->description, out);
			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}
	out << YAML::EndMap;

	std::ofstream fout("Assets/Data/AbilityData.yaml");
	fout << out.c_str();
	fout.close();
}

