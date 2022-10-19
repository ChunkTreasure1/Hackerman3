#pragma once
#include "Volt/Core/Base.h"
#include "Volt/Asset/Asset.h"
#include "Volt/Audio/AudioEngine.h"

#include <Wire/Wire.h>
#include <gem/gem.h>

#include <map>

namespace Volt
{
	class ParticleSystem;
	class Entity;
	class TextureCube;

	class Animation;
	class Skeleton;
	class AnimatedCharacter;
	class Event;
	class Image2D;

	class PhysicsSystem;

	struct SceneEnvironment
	{
		Ref<Image2D> irradianceMap;
		Ref<Image2D> radianceMap;
	};

	class Scene : public Asset
	{
	public:
		struct Statistics
		{
			uint32_t entityCount = 0;
		};

		struct TRS
		{
			gem::vec3 position;
			gem::vec3 rotation;
			gem::vec3 scale;
		};

		Scene();
		Scene(const std::string& name);

		inline Wire::Registry& GetRegistry() { return myRegistry; }
		inline const std::string& GetName() const { return myName; }
		inline const Statistics& GetStatistics() const { return myStatistics; }
		inline const bool IsPlaying() const { return myIsPlaying; }

		void OnRuntimeStart();
		void OnRuntimeEnd();

		void OnSimulationStart();
		void OnSimulationEnd();

		void Update(float aDeltaTime);
		void UpdateEditor(float aDeltaTime);
		void UpdateSimulation(float aDeltaTime);
		void OnEvent(Event& e);

		void SetRenderSize(uint32_t aWidth, uint32_t aHeight);

		Entity CreateEntity();
		void RemoveEntity(Entity entity);
		void RemoveEntity(Entity entity, float aTimeToDestroy);

		void ParentEntity(Entity parent, Entity child);
		void UnparentEntity(Entity entity);
		
		gem::mat4 GetWorldSpaceTransform(Entity entity);
		TRS GetWorldSpaceTRS(Entity entity);

		Entity InstantiateSplitMesh(const std::filesystem::path& path);

		gem::vec3 GetWorldForward(Entity entity);
		gem::vec3 GetWorldRight(Entity entity);
		gem::vec3 GetWorldUp(Entity entity);

		static AssetType GetStaticType() { return AssetType::Scene; }
		AssetType GetType() { return GetStaticType(); }

		void CopyTo(Ref<Scene> otherScene);

		Ref<ParticleSystem> myParticleSystem;
	private:
		friend class Entity;
		friend class SceneImporter;

		void SetupComponentCreationFunctions();
		void SetupComponentDeletionFunctions();

		SceneEnvironment myEnvironment;
		Statistics myStatistics;

		Ref<PhysicsSystem> myPhysicsSystem;

		bool myIsPlaying = false;
		float myTimeSinceStart = 0.f;

		std::string myName = "New Scene";
		Wire::Registry myRegistry;

		std::unordered_map<Wire::EntityId, FMOD::Studio::EventInstance> myAudioClips;
		std::map<Wire::EntityId, float> myEntityTimesToDestroy;

		FMOD::Studio::EventInstance* instance;

		uint32_t myWidth = 1;
		uint32_t myHeight = 1;
	};
}