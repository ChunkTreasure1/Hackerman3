#pragma once

#include "Volt/Asset/Asset.h"
#include "Volt/Asset/Mesh/Mesh.h"
#include "Volt/AI/NavMesh/NavMeshAgent.h"
#include "Volt/Rendering/Camera/Camera.h"
#include "Volt/Particles/Particle.h"


#include "Volt/Scene/Scene.h"

#include <Wire/Serialization.h>
#include <gem/gem.h>
#include <string>
namespace Volt
{
	SERIALIZE_COMPONENT((struct TagComponent
	{
		PROPERTY(Name = Tag) std::string tag;

		CREATE_COMPONENT_GUID("{282FA5FB-6A77-47DB-8340-3D34F1A1FBBD}"_guid);
	}), TagComponent);

	SERIALIZE_COMPONENT((struct TransformComponent
	{
		PROPERTY(Name = Position) gem::vec3 position;
		PROPERTY(Name = Rotation) gem::vec3 rotation;
		PROPERTY(Name = Scale) gem::vec3 scale;

		PROPERTY(Visible = false) bool visible = true;
		PROPERTY(Visible = false) bool locked = false;

		inline const gem::mat4 GetTransform() const
		{
			return gem::translate(gem::mat4(1.f), position) *
				gem::mat4_cast(gem::quat(rotation)) * gem::scale(gem::mat4(1.f), scale);
		}

		inline const gem::vec3 GetForward() const
		{
			const gem::quat orientation = gem::quat(rotation);
			return gem::rotate(orientation, gem::vec3{ 0.f, 0.f, 1.f });
		}

		inline const gem::vec3 GetRight() const
		{
			const gem::quat orientation = gem::quat(rotation);
			return gem::rotate(orientation, gem::vec3{ 1.f, 0.f, 0.f });
		}

		inline const gem::vec3 GetUp() const
		{
			const gem::quat orientation = gem::quat(rotation);
			return gem::rotate(orientation, gem::vec3{ 0.f, 1.f, 0.f });
		}

		CREATE_COMPONENT_GUID("{E1B8016B-1CAA-4782-927E-C17C29B25893}"_guid);
	}), TransformComponent);

	SERIALIZE_COMPONENT((struct RelationshipComponent
	{
		PROPERTY(Name = Children) std::vector<Wire::EntityId> Children;
		PROPERTY(Name = Parent) Wire::EntityId Parent = 0;
		CREATE_COMPONENT_GUID("{4A5FEDD2-4D0B-4696-A9E6-DCDFFB25B32C}"_guid);

	}), RelationshipComponent);

	SERIALIZE_COMPONENT((struct MeleeEnemyComponent
	{
		PROPERTY(Name = Activation Distance) float activationDistance;
		PROPERTY(Name = Speed) float moveSpeed;
		PROPERTY(Name = Attack Distance Percent) float attackP;
		PROPERTY(Name = Buffed) bool isBuffed;
		PROPERTY(Name = Champion) bool isChampion;
		PROPERTY(Name = BuffedHealthIncrease) float buffedHealthIncrease = 2;

		CREATE_COMPONENT_GUID("{B3529E68-5D4E-4174-8555-5382D0923A8C}"_guid);
	}), MeleeEnemyComponent);

	SERIALIZE_COMPONENT((struct BruiserEnemyComponent
	{
		PROPERTY(Name = Activation Distance) float activationDistance;
		PROPERTY(Name = Speed) float moveSpeed;
		PROPERTY(Name = Attack Distance Percent) float attackP;
		PROPERTY(Name = Buffed) bool isBuffed;
		PROPERTY(Name = BuffedHealthIncrease) float buffedHealthIncrease;

		CREATE_COMPONENT_GUID("{AC8B0500-4307-46AC-B112-EA28DFE9ACF0}"_guid);
	}), BruiserEnemyComponent);

	SERIALIZE_COMPONENT((struct BossComponent
	{
		PROPERTY(Name = Phase2percent) float phase2p;
		PROPERTY(Name = Phase3percent) float phase3p;
		PROPERTY(Name = KnockbackProcTime) float knockbackProc;
		PROPERTY(Name = KnockbackTimerResetDistance) float knockbackResetDistance;
		PROPERTY(TentacleSpawnDistance = tentacleSpawnDistance) float tentacleSpawnDistance;

		CREATE_COMPONENT_GUID("{9E579E1B-1FE4-44BC-8832-FE9F51F2C6AC}"_guid);
	}), BossComponent);


	SERIALIZE_COMPONENT((struct RangedEnemyComponent
	{
		PROPERTY(Name = Activation Distance) float activationDistance;
		PROPERTY(Name = Speed) float moveSpeed;
		PROPERTY(Name = Enter Attack Distance Percent) float enterAttack;
		PROPERTY(Name = Flee Attack Distance Percent) float fleeAttack;
		PROPERTY(Name = Flee Distance Percent) float fleeDistance;
		PROPERTY(Name = Buffed) bool isBuffed;
		PROPERTY(Name = BuffedHealthIncrease) float buffedHealthIncrease = 2;

		CREATE_COMPONENT_GUID("{772FAC29-6D33-4053-87E7-B2EED6225327}"_guid);
	}), RangedEnemyComponent);

	SERIALIZE_COMPONENT((struct PrefabComponent)
	{
		PROPERTY(Name = PrefabAsset, Visible = false) AssetHandle prefabAsset = Asset::Null();
		PROPERTY(Name = prefabEntity, Visible = false) Wire::EntityId prefabEntity = Wire::NullID;

		CREATE_COMPONENT_GUID("{B8A83ACF-F1CA-4C9F-8D1E-408B5BB388D2}"_guid);
	}, PrefabComponent);

	SERIALIZE_COMPONENT((struct ParticleEmitterComponent
	{
		PROPERTY(Name = Preset) AssetHandle preset;
		AssetHandle currentPreset;

		float emittionTimer = 0;
		int numberOfAliveParticles = 0;
		std::vector<Particle> particles;

		CREATE_COMPONENT_GUID("{E31271AB-47C7-4D6A-91E8-4B1A62B20D66}"_guid);
	}), ParticleEmitterComponent);

	SERIALIZE_COMPONENT((struct HealthComponent
	{
		PROPERTY(Name = Health) int health;
		int maxHealth;

		CREATE_COMPONENT_GUID("{DF26B5AA-D786-4DFF-9136-7E03537D1003}"_guid);
	}), HealthComponent);
	SERIALIZE_COMPONENT((struct MeshComponent
	{
		PROPERTY(Name = Mesh) AssetHandle handle = Asset::Null();
		PROPERTY(Name = Material) AssetHandle overrideMaterial = Asset::Null();
		PROPERTY(Name = Walkable) bool walkable = true;

		int32_t subMeshIndex = -1;
		int32_t subMaterialIndex = -1;

		CREATE_COMPONENT_GUID("{45D008BE-65C9-4D6F-A0C6-377F7B384E47}"_guid)
	}), MeshComponent);

	SERIALIZE_COMPONENT((struct AudioComponent
	{
		PROPERTY(Name = Event Path) std::string eventPath;
		PROPERTY(Name = Volume) float volume = 0.5f;
		PROPERTY(Name = Should Loop) bool shouldLoop = false;
		PROPERTY(Name = Play On Awake) bool playOnAwake = false;
		PROPERTY(Name = Three Dimensional) bool threeDimenstional = false;

		CREATE_COMPONENT_GUID("{D5161BC8-E96F-4E68-8752-CC8202575287}"_guid)
	}), AudioComponent);

	SERIALIZE_COMPONENT((struct DirectionalLightComponent
	{
		PROPERTY(Name = Intensity) float intensity = 1.f;
		PROPERTY(Name = Color, SpecialType = Color) gem::vec3 color = { 1.f, 1.f, 1.f };
		PROPERTY(Name = Cast Shadows) bool castShadows = true;

		CREATE_COMPONENT_GUID("{EC5514FF-9DE7-44CA-BCD9-8A9F08883F59}"_guid)
	}), DirectionalLightComponent);

	SERIALIZE_COMPONENT((struct PointLightComponent
	{
		PROPERTY(Name = Intensity) float intensity = 1.f;
		PROPERTY(Name = Radius) float radius = 100.f;
		PROPERTY(Name = Falloff) float falloff = 1.f;
		PROPERTY(Name = Far plane) float farPlane = 100.f;
		PROPERTY(Name = Color, SpecialType = Color) gem::vec3 color = { 1.f, 1.f, 1.f };

		CREATE_COMPONENT_GUID("{A30A8848-A30B-41DD-80F9-4E163C01ABC2}"_guid)
	}), PointLightComponent);

	SERIALIZE_COMPONENT((struct ScriptComponent
	{
		PROPERTY(Name = Scripts) std::vector<WireGUID> scripts;

		CREATE_COMPONENT_GUID("{4FB7727F-BDBE-47E6-9C14-40ECBC5C7927}"_guid);
	}), ScriptComponent);

	SERIALIZE_COMPONENT((struct NavMeshModifierComponent
	{
		PROPERTY(Name = Active) bool active = true;
		CREATE_COMPONENT_GUID("{4C7EB050-3DDA-43D5-A417-1B82DF260EC7}"_guid);
	}), NavMeshModifierComponent);

	SERIALIZE_COMPONENT((struct NavMeshAgentComponent
	{
		NavMeshAgent agent;
		CREATE_COMPONENT_GUID("{F29BA549-DD7D-407E-8024-6E281C4ED2AC}"_guid);
	}), NavMeshAgentComponent);

	SERIALIZE_COMPONENT((struct CameraComponent
	{
		CameraComponent()
		{
			camera = CreateRef<Camera>(fieldOfView, 16.f / 9.f, nearPlane, farPlane);
		}

		PROPERTY(Name = Field of View) float fieldOfView = 60.f;
		PROPERTY(Name = Near plane) float nearPlane = 1.f;
		PROPERTY(Name = Far plane) float farPlane = 100000.f;
		PROPERTY(Name = Priority) uint32_t priority = 0;
		PROPERTY(Name = SmoothTime) float smoothTime = 0.040f; //Good Value :)

		Ref<Camera> camera;

		CREATE_COMPONENT_GUID("{9258BEEC-3A31-4CAB-AB1E-654524E1C398}"_guid);
	}), CameraComponent);



	SERIALIZE_COMPONENT((struct PlayerComponent
	{
		PROPERTY(Name = Walk Speed) float walkSpeed = 400.f;
		PROPERTY(Name = Arrow Buffed) float isBuffed = false;
		PROPERTY(Name = Current Fury) int currentFury = 0;
		PROPERTY(Name = Max Fury) int maxFury = 250;
		PROPERTY(Name = Fury Decrease Amount) int furyDecreaseAmount = 3;
		PROPERTY(Name = Time Until Fury Decrease) float timeUntilFuryDecrese = 5.f;
		PROPERTY(Name = Attack Stop Distance) float attackStopDist = 5.f;

		float furyDecreaseTime = timeUntilFuryDecrese;

		CREATE_COMPONENT_GUID("{979D1E44-927F-4FDA-9D42-8565D4DC4550}"_guid);
	}), PlayerComponent);

	SERIALIZE_COMPONENT((struct AnimatedCharacterComponent
	{
		PROPERTY(Name = Character) AssetHandle animatedCharacter = Asset::Null();

		uint32_t currentAnimation = 0;
		float currentStartTime = 0.f;
		bool isLooping = true;

		CREATE_COMPONENT_GUID("{37333031-9816-4DDE-BFEA-5E83E32754D1}"_guid);
	}), AnimatedCharacterComponent);

	SERIALIZE_COMPONENT((struct SkylightComponent
	{
		PROPERTY(Name = Environment Map) AssetHandle environmentHandle = Asset::Null();
		PROPERTY(Name = Intensity) float intensity = 1.f;

		AssetHandle lastEnvironmentHandle = Asset::Null();
		SceneEnvironment currentSceneEnvironment;

		CREATE_COMPONENT_GUID("{29F75381-2873-4734-A074-3F3640E54C84}"_guid);
	}), SkylightComponent);

	SERIALIZE_COMPONENT((struct DialogueTriggerComponent
	{
		PROPERTY(Name = DialogueFile) std::string DialogueFile;

		CREATE_COMPONENT_GUID("{D63FAB92-3B97-4E46-B64E-1E0D9BDFCCF4}"_guid);
	}), DialogueTriggerComponent);

	SERIALIZE_COMPONENT((struct LevelTransitionTriggerComponent
	{
		PROPERTY(Name = Level) AssetHandle handle = Asset::Null();

		CREATE_COMPONENT_GUID("{47DEC42C-9166-43FF-8212-41E8092AC369}"_guid);
	}), LevelTransitionTriggerComponent);

	SERIALIZE_COMPONENT((struct DestructableObjectComponent
	{
		PROPERTY(Name = SlicedMesh) AssetHandle handle = Asset::Null();

		CREATE_COMPONENT_GUID("{9884B2EE-11BB-40E2-9EB1-2EB58DBAD2CD}"_guid);
	}), DestructableObjectComponent);

	SERIALIZE_COMPONENT((struct EntityDataComponent
	{
		float timeSinceCreation = 0.f;

		CREATE_COMPONENT_GUID("{A6789316-2D82-46FC-8138-B7BCBB9EA5B8}"_guid);
	}), EntityDataComponent);

	SERIALIZE_COMPONENT((struct AbilityGainerComponent
	{
		PROPERTY(Name = Swipe) bool Swipe = false;
		PROPERTY(Name = Dash) bool Dash = false;
		PROPERTY(Name = Fear) bool Fear = false;
		PROPERTY(Name = BloodLust) bool BloodLust = false;
		PROPERTY(Name = BuffedArrow) bool BuffedArrow = false;

		CREATE_COMPONENT_GUID("{AF9DAB53-DEC8-405E-AD02-E47DE8C39B01}"_guid);
	}), AbilityGainerComponent);

	SERIALIZE_COMPONENT((struct TextRendererComponent
	{
		PROPERTY(Name = Text) std::string text = "Text";
		PROPERTY(Name = Font) AssetHandle fontHandle;
		PROPERTY(Name = Max Width) float maxWidth = 100.f;

		CREATE_COMPONENT_GUID("{8AAA0646-40D2-47E6-B83F-72EA26BD8C01}"_guid);
	}), TextRendererComponent);
}