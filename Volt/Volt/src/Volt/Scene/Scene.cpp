#include "vtpch.h"
#include "Scene.h"

#include "Volt/Asset/AssetManager.h"
#include "Volt/Asset/Animation/Animation.h"
#include "Volt/Asset/Animation/AnimatedCharacter.h"

#include "Volt/Log/Log.h"
#include "Volt/core/Profiling.h"
#include "Volt/Scene/Entity.h"
#include "Volt/Components/Components.h"
#include "Volt/Animation/AnimationManager.h"

#include "Volt/Physics/Physics.h"
#include "Volt/Physics/PhysicsScene.h"
#include "Volt/Physics/PhysicsSystem.h"
#include "Volt/Particles/ParticleSystem.h"

#include "Volt/Scripting/ScriptEngine.h"
#include "Volt/Scripting/ScriptBase.h"

#include "Volt/Utility/Math.h"
#include "Volt/Utility/FileSystem.h"

#include "Volt/Rendering/RendererStructs.h"
#include "Volt/Rendering/Renderer.h"

#include <Wire/Serialization.h>
#include "../Sandbox/src/Sandbox/EditorCommandStack.h"

namespace Volt
{
	Scene::Scene(const std::string& name)
		: myName(name)
	{
		myParticleSystem = CreateRef<ParticleSystem>(this);

		SetupComponentCreationFunctions();
		SetupComponentDeletionFunctions();
	}

	Scene::Scene()
	{
		myParticleSystem = CreateRef<ParticleSystem>(this);

		SetupComponentCreationFunctions();
		SetupComponentDeletionFunctions();
	}

	void Scene::OnEvent(Event& e)
	{
		if (!myIsPlaying)
		{
			return;
		}

		myRegistry.ForEach<ScriptComponent>([&](Wire::EntityId id, const ScriptComponent& scriptComp)
			{
				for (const auto& scriptId : scriptComp.scripts)
				{
					Ref<ScriptBase> scriptInstance = ScriptEngine::GetScript(id, scriptId);
					scriptInstance->OnEvent(e);
				}
			});
	}

	void Scene::SetRenderSize(uint32_t aWidth, uint32_t aHeight)
	{
		myWidth = aWidth;
		myHeight = aHeight;
	}

	void Scene::OnRuntimeStart()
	{
		Physics::CreateScene(this);
		Physics::CreateActors(this);

		myPhysicsSystem = CreateRef<PhysicsSystem>(this);

		myIsPlaying = true;
		myTimeSinceStart = 0.f;

		myRegistry.ForEach<EntityDataComponent>([](Wire::EntityId id, EntityDataComponent& dataComp)
			{
				dataComp.timeSinceCreation = 0.f;
			});

		myRegistry.ForEach<ScriptComponent>([this](Wire::EntityId id, const ScriptComponent& scriptComp)
			{
				for (const auto& scriptId : scriptComp.scripts)
				{
					Ref<ScriptBase> scriptInstance = ScriptRegistry::Create(ScriptRegistry::GetNameFromGUID(scriptId), Entity{ id, this });
					if (!scriptInstance)
					{
						VT_CORE_WARN("Unable to create script with name {0} on entity {1}!", ScriptRegistry::GetNameFromGUID(scriptId), id);
					}
					ScriptEngine::RegisterToEntity(scriptInstance, id);

					if (scriptInstance)
					{
						scriptInstance->OnAwake();
					}
					else
					{
						VT_CORE_WARN("Unable to create script with name {0} on entity {1}!", ScriptRegistry::GetNameFromGUID(scriptId), id);
					}

				}
			});

		myRegistry.ForEach<ScriptComponent>([](Wire::EntityId id, const ScriptComponent& scriptComp)
			{
				for (const auto& scriptId : scriptComp.scripts)
				{
					auto script = ScriptEngine::GetScript(id, scriptId);

					if (script)
					{
						script->OnStart();
					}
					else
					{
						VT_CORE_WARN("Unable to get script with name {0}!", ScriptRegistry::GetNameFromGUID(scriptId));
					}
				}
			});

		myRegistry.ForEach<AnimatedCharacterComponent>([](Wire::EntityId id, AnimatedCharacterComponent& animCharComp)
			{
				animCharComp.currentStartTime = AnimationManager::globalClock;
			});

		myRegistry.ForEach<AudioComponent>([this](Wire::EntityId id, const AudioComponent& audioComp)
			{
				if (!audioComp.eventPath.empty())
				{
					if (audioComp.playOnAwake)
					{
						instance = AUDIOENGINE.CreateEventInstance("event:/Test");
						AUDIOENGINE.PlayEvent(instance);
					}
				}
			});

	}

	void Scene::OnRuntimeEnd()
	{
		myIsPlaying = false;

		myRegistry.ForEach<ScriptComponent>([](Wire::EntityId id, const ScriptComponent& scriptComp)
			{
				for (const auto& scriptId : scriptComp.scripts)
				{
					auto script = ScriptEngine::GetScript(id, scriptId);
					script->OnDetach();
					script->OnStop();
				}
			});

		ScriptEngine::Clear();
		AUDIOENGINE.StopAll(0);

		myPhysicsSystem = nullptr;
		Physics::DestroyScene();
	}

	void Scene::OnSimulationStart()
	{
		Physics::CreateScene(this);
		Physics::CreateActors(this);

		myPhysicsSystem = CreateRef<PhysicsSystem>(this);
	}

	void Scene::OnSimulationEnd()
	{
		myPhysicsSystem = nullptr;
		Physics::DestroyScene();
	}

	void Scene::Update(float aDeltaTime)
	{
		VT_PROFILE_FUNCTION();
		myStatistics.entityCount = (uint32_t)myRegistry.GetAllEntities().size();

		Physics::GetScene()->Simulate(aDeltaTime);
		myPhysicsSystem->Update(aDeltaTime);

		myTimeSinceStart += aDeltaTime;

		// Update scene data
		{
			SceneData sceneData;
			sceneData.deltaTime = aDeltaTime;
			sceneData.timeSinceStart = myTimeSinceStart;

			Renderer::SetSceneData(sceneData);
		}

		myRegistry.ForEach<EntityDataComponent>([aDeltaTime](Wire::EntityId id, EntityDataComponent& dataComp) 
			{
				dataComp.timeSinceCreation += aDeltaTime;
			});

		myRegistry.ForEach<ScriptComponent>([aDeltaTime](Wire::EntityId id, const ScriptComponent& scriptComp)
			{
				for (const auto& scriptId : scriptComp.scripts)
				{
					auto script = ScriptEngine::GetScript(id, scriptId);

					if (script)
					{
						script->OnUpdate(aDeltaTime);
					}
					else
					{
						VT_CORE_WARN("Unable to get script with name {0}!", ScriptRegistry::GetNameFromGUID(scriptId));
					}
				}
			});

		myRegistry.ForEach<AudioComponent>([aDeltaTime, this](Wire::EntityId id, const AudioComponent& audioComp)
			{
				// AudioComponent update.
			});

		myRegistry.ForEach<CameraComponent, TransformComponent>([this](Wire::EntityId id, CameraComponent& cameraComp, const TransformComponent& transComp)
			{
				if (transComp.visible)
				{
					cameraComp.camera->SetPerspectiveProjection(cameraComp.fieldOfView, (float)myWidth / (float)myHeight, cameraComp.nearPlane, cameraComp.farPlane);
					cameraComp.camera->SetPosition(transComp.position);
					cameraComp.camera->SetRotation(transComp.rotation);
				}
			});

		myRegistry.ForEach<AnimatedCharacterComponent>([](Wire::EntityId id, AnimatedCharacterComponent& animCharComp)
			{
				if (animCharComp.animatedCharacter != Asset::Null())
				{
					auto animChar = AssetManager::GetAsset<AnimatedCharacter>(animCharComp.animatedCharacter);
					if (animChar && animChar->IsValid())
					{
						if (animCharComp.isLooping && animCharComp.currentStartTime + animChar->GetAnimationDuration(animCharComp.currentAnimation) <= AnimationManager::globalClock)
						{
							animCharComp.currentStartTime = AnimationManager::globalClock;
						}
					}
				}

			});

		myParticleSystem->Update(aDeltaTime);

		for (auto& [ent, time] : myEntityTimesToDestroy)
		{
			time -= aDeltaTime;
			if (time <= 0.f)
			{
				RemoveEntity(Entity{ ent, this });
			}
		}


		for (auto it = myEntityTimesToDestroy.begin(); it != myEntityTimesToDestroy.end();)
		{
			if (it->second <= 0.f)
			{
				it = myEntityTimesToDestroy.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	void Scene::UpdateEditor(float aDeltaTime)
	{
		myStatistics.entityCount = (uint32_t)myRegistry.GetAllEntities().size();

		// Update scene data
		{
			SceneData sceneData;
			sceneData.deltaTime = aDeltaTime;
			sceneData.timeSinceStart = myTimeSinceStart;

			Renderer::SetSceneData(sceneData);
		}
	}

	void Scene::UpdateSimulation(float aDeltaTime)
	{
		Physics::GetScene()->Simulate(aDeltaTime);
		myPhysicsSystem->Update(aDeltaTime);

		myStatistics.entityCount = (uint32_t)myRegistry.GetAllEntities().size();

		// Update scene data
		{
			SceneData sceneData;
			sceneData.deltaTime = aDeltaTime;
			sceneData.timeSinceStart = myTimeSinceStart;

			Renderer::SetSceneData(sceneData);
		}
	}

	Entity Scene::CreateEntity()
	{
		Wire::EntityId id = myRegistry.CreateEntity();

		Entity newEntity = Entity(id, this);
		auto& transform = newEntity.AddComponent<TransformComponent>();
		transform.position = { 0.f, 0.f, 0.f };
		transform.rotation = { 0.f, 0.f, 0.f };
		transform.scale = { 1.f, 1.f, 1.f };

		auto& tag = newEntity.AddComponent<TagComponent>();
		tag.tag = "New Entity";

		newEntity.AddComponent<RelationshipComponent>();
		newEntity.AddComponent<EntityDataComponent>();

		std::vector<Volt::Entity> entities;
		entities.push_back(newEntity);

		return newEntity;
	}

	void Scene::RemoveEntity(Entity entity)
	{
		if (!myRegistry.Exists(entity.GetId()))
		{
			return;
		}

		if (myIsPlaying && entity.HasComponent<ScriptComponent>())
		{
			auto& scriptComp = entity.GetComponent<ScriptComponent>();
			for (const auto& script : scriptComp.scripts)
			{
				ScriptEngine::UnregisterFromEntity(script, entity.GetId());
			}
		}

		if (entity.HasComponent<RelationshipComponent>())
		{
			auto& relComp = entity.GetComponent<RelationshipComponent>();
			if (relComp.Parent)
			{
				Entity parentEnt{ relComp.Parent, this };

				if (parentEnt.HasComponent<RelationshipComponent>())
				{
					auto& parentRelComp = parentEnt.GetComponent<RelationshipComponent>();
					auto it = std::find(parentRelComp.Children.begin(), parentRelComp.Children.end(), entity.GetId());
					parentRelComp.Children.erase(it);
				}
			}

			for (const auto& child : relComp.Children)
			{
				Entity childEnt{ child, this };
				RemoveEntity(childEnt);
			}
		}

		myRegistry.RemoveEntity(entity.GetId());
	}

	void Scene::RemoveEntity(Entity entity, float aTimeToDestroy)
	{
		if (myEntityTimesToDestroy.find(entity.GetId()) != myEntityTimesToDestroy.end())
		{
			return;
		}

		myEntityTimesToDestroy.emplace(entity.GetId(), aTimeToDestroy);
	}

	void Scene::ParentEntity(Entity parent, Entity child)
	{
		if (parent.IsNull() || child.IsNull() || parent == child) 
		{ 
			return; 
		}
		//gem::vec3 childWorldSpacePosition = child.GetPosition();
		//gem::vec3 parentWorldSpacePosition = parent.GetPosition();
		//gem::vec3 difference = parentWorldSpacePosition - childWorldSpacePosition;

		UnparentEntity(child);
		auto& children = parent.GetComponent<RelationshipComponent>().Children;
		auto& childChildren = child.GetComponent<RelationshipComponent>().Children;

		if (auto it = std::find(childChildren.begin(), childChildren.end(), parent.GetId()) != childChildren.end())
		{
			return;
		}

		child.GetComponent<RelationshipComponent>().Parent = parent.GetId();
		parent.GetComponent<RelationshipComponent>().Children.emplace_back(child.GetId());
		child.GetComponent<TransformComponent>().position -= parent.GetPosition();
	}

	void Scene::UnparentEntity(Entity entity)
	{
		if (entity.IsNull()) { return; }

		auto parent = Entity(entity.GetComponent<RelationshipComponent>().Parent, this);
		if (!parent.IsNull())
		{
			auto& children = parent.GetComponent<RelationshipComponent>().Children;

			auto it = std::find(children.begin(), children.end(), entity.GetId());
			if (it != children.end())
			{
				children.erase(it);
			}

		}
		entity.GetComponent<RelationshipComponent>().Parent = Wire::NullID;
		entity.GetComponent<TransformComponent>().position += parent.GetPosition();
	}

	gem::mat4 Scene::GetWorldSpaceTransform(Entity entity)
	{
		gem::mat4 transform(1.0f);
		Wire::EntityId parentId = Wire::NullID;

		if (entity.HasComponent<RelationshipComponent>())
		{
			parentId = entity.GetComponent<RelationshipComponent>().Parent;
		}

		if (parentId)
		{
			Entity parent(parentId, this);
			transform = GetWorldSpaceTransform(parent);
		}

		const auto& transformComp = entity.GetComponent<TransformComponent>();

		return transform * transformComp.GetTransform();
	}

	Scene::TRS Scene::GetWorldSpaceTRS(Entity entity)
	{
		TRS transform;

		gem::decompose(GetWorldSpaceTransform(entity), transform.position, transform.rotation, transform.scale);
		return transform;
	}

	Entity Scene::InstantiateSplitMesh(const std::filesystem::path& path)
	{
		auto mesh = AssetManager::GetAsset<Mesh>(path);
		if (!mesh || !mesh->IsValid())
		{
			VT_CORE_WARN("Trying to instantiate invalid mesh {0}!", path.string());
			return Entity{};
		}

		Entity parentEntity = CreateEntity();

		for (uint32_t i = 0; const auto & subMesh : mesh->GetSubMeshes())
		{
			Entity childEntity = CreateEntity();
			ParentEntity(parentEntity, childEntity);

			auto& meshComponent = childEntity.AddComponent<MeshComponent>();
			meshComponent.handle = mesh->handle;
			meshComponent.subMeshIndex = i;
			i++;
		}

		return parentEntity;
	}

	gem::vec3 Scene::GetWorldForward(Entity entity)
	{
		gem::vec3 p, r, s;
		Math::DecomposeTransform(GetWorldSpaceTransform(entity), p, r, s);

		const gem::quat orientation = gem::quat(r);
		return gem::rotate(orientation, gem::vec3{ 0.f, 0.f, 1.f });
	}

	gem::vec3 Scene::GetWorldRight(Entity entity)
	{
		gem::vec3 p, r, s;
		Math::DecomposeTransform(GetWorldSpaceTransform(entity), p, r, s);

		const gem::quat orientation = gem::quat(r);
		return gem::rotate(orientation, gem::vec3{ 1.f, 0.f, 0.f });
	}

	gem::vec3 Scene::GetWorldUp(Entity entity)
	{
		gem::vec3 p, r, s;
		Math::DecomposeTransform(GetWorldSpaceTransform(entity), p, r, s);

		const gem::quat orientation = gem::quat(r);
		return gem::rotate(orientation, gem::vec3{ 0.f, 1.f, 0.f });
	}

	void Scene::CopyTo(Ref<Scene> otherScene)
	{
		VT_PROFILE_FUNCTION();

		otherScene->myName = myName;
		otherScene->myEnvironment = myEnvironment;

		auto& otherRegistry = otherScene->GetRegistry();

		// Copy registry
		for (const auto& ent : myRegistry.GetAllEntities())
		{
			otherRegistry.AddEntity(ent);
			Entity::Copy(myRegistry, otherRegistry, ent, ent);
		}
	}

	void Scene::SetupComponentCreationFunctions()
	{
		myRegistry.SetOnCreateFunction<RigidbodyComponent>([&](Wire::EntityId id, void* compPtr)
			{
				if (!myIsPlaying)
				{
					return;
				}

				Physics::CreateActor(Entity{ id, this });
			});

		myRegistry.SetOnCreateFunction<BoxColliderComponent>([&](Wire::EntityId id, void* compPtr)
			{
				if (!myIsPlaying)
				{
					return;
				}

				auto entity = Entity{ id, this };
				auto actor = Physics::GetScene()->GetActor(entity);
				BoxColliderComponent& comp = (BoxColliderComponent&)compPtr;

				if (actor && !comp.added)
				{
					actor->AddCollider(comp, entity);
				}
			});

		myRegistry.SetOnCreateFunction<SphereColliderComponent>([&](Wire::EntityId id, void* compPtr)
			{
				if (!myIsPlaying)
				{
					return;
				}

				auto entity = Entity{ id, this };
				auto actor = Physics::GetScene()->GetActor(entity);
				SphereColliderComponent& comp = (SphereColliderComponent&)compPtr;

				if (actor && !comp.added)
				{
					actor->AddCollider(comp, entity);
				}
			});

		myRegistry.SetOnCreateFunction<CapsuleColliderComponent>([&](Wire::EntityId id, void* compPtr)
			{
				if (!myIsPlaying)
				{
					return;
				}

				auto entity = Entity{ id, this };
				auto actor = Physics::GetScene()->GetActor(entity);
				CapsuleColliderComponent& comp = (CapsuleColliderComponent&)compPtr;

				if (actor && !comp.added)
				{
					actor->AddCollider((CapsuleColliderComponent&)compPtr, entity);
				}
			});

		myRegistry.SetOnCreateFunction<MeshColliderComponent>([&](Wire::EntityId id, void* compPtr)
			{
				if (!myIsPlaying)
				{
					return;
				}

				auto entity = Entity{ id, this };
				auto actor = Physics::GetScene()->GetActor(entity);
				MeshColliderComponent& comp = (MeshColliderComponent&)compPtr;

				if (actor && !comp.added)
				{
					actor->AddCollider(comp, entity);
				}
			});
	}
	void Scene::SetupComponentDeletionFunctions()
	{
		myRegistry.SetOnRemoveFunction<RigidbodyComponent>([&](Wire::EntityId id, void* compPtr)
			{
				if (!myIsPlaying)
				{
					return;
				}

				auto actor = Physics::GetScene()->GetActor(Entity{ id, this });
				if (actor)
				{
					Physics::GetScene()->RemoveActor(actor);
				}
			});

		myRegistry.SetOnRemoveFunction<BoxColliderComponent>([&](Wire::EntityId id, void* compPtr)
			{
				if (!myIsPlaying)
				{
					return;
				}

				auto actor = Physics::GetScene()->GetActor(Entity{ id, this });
				if (actor)
				{
					actor->RemoveCollider(ColliderType::Box);
				}
			});

		myRegistry.SetOnRemoveFunction<SphereColliderComponent>([&](Wire::EntityId id, void* compPtr)
			{
				if (!myIsPlaying)
				{
					return;
				}

				auto actor = Physics::GetScene()->GetActor(Entity{ id, this });
				if (actor)
				{
					actor->RemoveCollider(ColliderType::Sphere);
				}
			});

		myRegistry.SetOnRemoveFunction<CapsuleColliderComponent>([&](Wire::EntityId id, void* compPtr)
			{
				if (!myIsPlaying)
				{
					return;
				}

				auto actor = Physics::GetScene()->GetActor(Entity{ id, this });
				if (actor)
				{
					actor->RemoveCollider(ColliderType::Capsule);
				}
			});

		myRegistry.SetOnRemoveFunction<MeshColliderComponent>([&](Wire::EntityId id, void* compPtr)
			{
				if (!myIsPlaying)
				{
					return;
				}

				auto actor = Physics::GetScene()->GetActor(Entity{ id, this });
				if (actor)
				{
					auto& comp = (MeshColliderComponent&)compPtr;
					if (comp.isConvex)
					{
						actor->RemoveCollider(ColliderType::ConvexMesh);
					}
					else
					{
						actor->RemoveCollider(ColliderType::TriangleMesh);
					}
				}
			});
	}
}