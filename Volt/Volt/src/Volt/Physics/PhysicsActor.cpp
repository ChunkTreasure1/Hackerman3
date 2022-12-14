#include "vtpch.h"
#include "PhysicsActor.h"

#include "Volt/Physics/PhysXInternal.h"
#include "Volt/Physics/PhysXUtilities.h"
#include "Volt/Physics/Physics.h"

#include "Volt/Physics/PhysicsShapes.h"
#include "Volt/Physics/PhysicsLayer.h"

#include "Volt/Components/Components.h"

#include "Volt/Log/Log.h"

namespace Volt
{
	PhysicsActor::PhysicsActor(Entity entity)
		: myEntity(entity)
	{
		myRigidBodyData = myEntity.GetComponent<RigidbodyComponent>();
		myEntity.GetComponent<RigidbodyComponent>().UpdateLast();
		CreateRigidActor();
	}

	PhysicsActor::~PhysicsActor()
	{}

	void PhysicsActor::SetLinearDrag(float drag) const
	{
		if (!IsDynamic())
		{
			VT_CORE_WARN("Trying to set drag of non-dynamic PhysicsActor!");
			return;
		}

		physx::PxRigidDynamic* actor = myRigidActor->is<physx::PxRigidDynamic>();
		VT_CORE_ASSERT(actor, "Actor is null!");
		
		actor->setLinearDamping(drag);
	}

	void PhysicsActor::SetAngularDrag(float drag) const
	{
		if (!IsDynamic())
		{
			VT_CORE_WARN("Trying to set drag of non-dynamic PhysicsActor!");
			return;
		}

		physx::PxRigidDynamic* actor = myRigidActor->is<physx::PxRigidDynamic>();
		VT_CORE_ASSERT(actor, "Actor is null!");

		actor->setAngularDamping(drag);
	}

	void PhysicsActor::SetGravityDisabled(bool disabled) const
	{
		myRigidActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, disabled);
	}

	void PhysicsActor::SetMass(float mass)
	{
		if (!IsDynamic())
		{
			VT_CORE_WARN("Static PhysicsActor can't have mass!");
			return;
		}
		myRigidBodyData.mass = mass;

		physx::PxRigidDynamic* actor = myRigidActor->is<physx::PxRigidDynamic>();
		VT_CORE_ASSERT(actor, "Actor is null!");
	
		physx::PxRigidBodyExt::setMassAndUpdateInertia(*actor, mass);
	}

	void PhysicsActor::SetKinematic(bool isKinematic)
	{
		if (!IsDynamic())
		{
			VT_CORE_WARN("Static PhysicsActor can't be kinematic!");
			return;
		}
		myRigidBodyData.isKinematic = isKinematic;

		physx::PxRigidDynamic* actor = myRigidActor->is<physx::PxRigidDynamic>();
		VT_CORE_ASSERT(actor, "Actor is null!");
	
		actor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, isKinematic);
	}

	void PhysicsActor::SetSimulationData(uint32_t layerId)
	{
		const auto data = PhysXUtilities::CreateFilterDataFromLayer(layerId, (CollisionDetectionType)myRigidBodyData.collisionType);
		myFilterData = data;

		for (auto& collider : myColliders)
		{
			collider->SetFilterData(myFilterData);
		}
	}

	void PhysicsActor::SetKinematicTarget(const gem::vec3& position, const gem::vec3& rotation)
	{
		if (!IsKinematic())
		{
			VT_CORE_WARN("Trying to set kinematic target for a non-kinematic actor.");
			return;
		}

		physx::PxRigidDynamic* actor = myRigidActor->is<physx::PxRigidDynamic>();
		VT_CORE_ASSERT(actor, "Actor is null!");
		actor->setKinematicTarget(PhysXUtilities::ToPhysXTransform(position, rotation));
	}

	void PhysicsActor::SetLinearVelocity(const gem::vec3 & velocity)
	{
		if (!IsDynamic())
		{
			VT_CORE_WARN("Trying to set velocity of non-dynamic PhysicsActor");
			return;
		}

		physx::PxRigidDynamic* actor = myRigidActor->is<physx::PxRigidDynamic>();
		VT_CORE_ASSERT(actor, "Actor is null!");

		actor->setLinearVelocity(PhysXUtilities::ToPhysXVector(velocity));
	}

	void PhysicsActor::SetAngularVelocity(const gem::vec3 & velocity)
	{
		if (!IsDynamic())
		{
			VT_CORE_WARN("Trying to set velocity of non-dynamic PhysicsActor");
			return;
		}

		physx::PxRigidDynamic* actor = myRigidActor->is<physx::PxRigidDynamic>();
		VT_CORE_ASSERT(actor, "Actor is null!");

		actor->setAngularVelocity(PhysXUtilities::ToPhysXVector(velocity));
	}

	void PhysicsActor::SetMaxLinearVelocity(float velocity)
	{
		if (!IsDynamic())
		{
			VT_CORE_WARN("Trying to set velocity of non-dynamic PhysicsActor");
			return;
		}

		physx::PxRigidDynamic* actor = myRigidActor->is<physx::PxRigidDynamic>();
		VT_CORE_ASSERT(actor, "Actor is null!");

		actor->setMaxLinearVelocity(velocity);
	}

	void PhysicsActor::SetMaxAngularVelocity(float velocity)
	{
		if (!IsDynamic())
		{
			VT_CORE_WARN("Trying to set velocity of non-dynamic PhysicsActor");
			return;
		}

		physx::PxRigidDynamic* actor = myRigidActor->is<physx::PxRigidDynamic>();
		VT_CORE_ASSERT(actor, "Actor is null!");

		actor->setMaxAngularVelocity(velocity);
	}

	const float PhysicsActor::GetMass() const
	{
		return !IsDynamic() ? myRigidBodyData.mass: myRigidActor->is<physx::PxRigidDynamic>()->getMass();
	}

	const gem::vec3 PhysicsActor::GetLinearVelocity() const
	{
		if (!IsDynamic())
		{
			VT_CORE_WARN("Trying to get velocity of non-dynamic PhysicsActor");
			return gem::vec3(0.f);
		}

		physx::PxRigidDynamic* actor = myRigidActor->is<physx::PxRigidDynamic>();
		VT_CORE_ASSERT(actor, "Actor is null!");

		return PhysXUtilities::FromPhysXVector(actor->getLinearVelocity());
	}

	const gem::vec3 PhysicsActor::GetAngularVelocity() const
	{
		if (!IsDynamic())
		{
			VT_CORE_WARN("Trying to get velocity of non-dynamic PhysicsActor");
			return gem::vec3(0.f);
		}

		physx::PxRigidDynamic* actor = myRigidActor->is<physx::PxRigidDynamic>();
		VT_CORE_ASSERT(actor, "Actor is null!");

		return PhysXUtilities::FromPhysXVector(actor->getAngularVelocity());
	}

	const float PhysicsActor::GetMaxLinearVelocity() const
	{
		if (!IsDynamic())
		{
			VT_CORE_WARN("Trying to get velocity of non-dynamic PhysicsActor");
			return 0.f;
		}

		physx::PxRigidDynamic* actor = myRigidActor->is<physx::PxRigidDynamic>();
		VT_CORE_ASSERT(actor, "Actor is null!");

		return actor->getMaxLinearVelocity();
	}

	const float PhysicsActor::GetMaxAngularVelocity() const
	{
		if (!IsDynamic())
		{
			VT_CORE_WARN("Trying to get velocity of non-dynamic PhysicsActor");
			return 0.f;
		}

		physx::PxRigidDynamic* actor = myRigidActor->is<physx::PxRigidDynamic>();
		VT_CORE_ASSERT(actor, "Actor is null!");

		return actor->getMaxAngularVelocity();
	}

	const gem::vec3 PhysicsActor::GetKinematicTargetPosition() const
	{
		if (!IsKinematic())
		{
			VT_CORE_WARN("Trying to set kinematic target for a non-kinematic actor.");
			return gem::vec3(0.0f, 0.0f, 0.0f);
		}

		physx::PxRigidDynamic* actor = myRigidActor->is<physx::PxRigidDynamic>();
		VT_CORE_ASSERT(actor, "Actor is null!");
		physx::PxTransform target;
		actor->getKinematicTarget(target);
		return PhysXUtilities::FromPhysXVector(target.p);
	}

	const gem::vec3 PhysicsActor::GetKinematicTargetRotation() const
	{
		if (!IsKinematic())
		{
			VT_CORE_WARN("Trying to set kinematic target for a non-kinematic actor.");
			return gem::vec3(0.0f, 0.0f, 0.0f);
		}

		physx::PxRigidDynamic* actor = myRigidActor->is<physx::PxRigidDynamic>();
		VT_CORE_ASSERT(actor, "Actor is null!");
		physx::PxTransform target;
		actor->getKinematicTarget(target);
		return gem::eulerAngles(PhysXUtilities::FromPhysXQuat(target.q));
	}

	void PhysicsActor::SetPosition(const gem::vec3& position, bool autoWake)
	{
		physx::PxTransform transform = myRigidActor->getGlobalPose();
		transform.p = PhysXUtilities::ToPhysXVector(position);

		myRigidActor->setGlobalPose(transform);

		if (myRigidBodyData.bodyType == BodyType::Static)
		{
			SynchronizeTransform();
		}
	}

	void PhysicsActor::SetRotation(const gem::vec3 & rotation, bool autoWake)
	{
		physx::PxTransform transform = myRigidActor->getGlobalPose();
		transform.q = PhysXUtilities::ToPhysXQuat(gem::quat(rotation));

		myRigidActor->setGlobalPose(transform);

		if (myRigidBodyData.bodyType == BodyType::Static)
		{
			SynchronizeTransform();
		}
	}

	Ref<ColliderShape> PhysicsActor::GetColliderOfType(ColliderType aType) const
	{
		for (const auto& coll : myColliders)
		{
			if (coll->GetType() == aType)
			{
				return coll;
			}
		}
		return nullptr;
	}

	void PhysicsActor::AddForce(const gem::vec3& aForce, ForceMode aForceMode)
	{
		if (!IsDynamic())
		{
			VT_CORE_WARN("Trying to add force to non-dynamic PhysicsActor");
			return;
		}

		physx::PxRigidDynamic* actor = myRigidActor->is<physx::PxRigidDynamic>();
		VT_CORE_ASSERT(actor, "Actor is null!");
		actor->addForce(PhysXUtilities::ToPhysXVector(aForce), (physx::PxForceMode::Enum)aForceMode);
	}

	void PhysicsActor::AddTorque(const gem::vec3 & torque, ForceMode aForceMode)
	{
		if (!IsDynamic())
		{
			VT_CORE_WARN("Trying to add torque to non-dynamic PhysicsActor");
			return;
		}

		physx::PxRigidDynamic* actor = myRigidActor->is<physx::PxRigidDynamic>();
		VT_CORE_ASSERT(actor, "Actor is null!");
		actor->addTorque(PhysXUtilities::ToPhysXVector(torque), (physx::PxForceMode::Enum)aForceMode);
	}

	void PhysicsActor::WakeUp()
	{
		if (IsDynamic())
		{
			myRigidActor->is<physx::PxRigidDynamic>()->wakeUp();
		}
	}

	void PhysicsActor::PutToSleep()
	{
		if (IsDynamic())
		{
			myRigidActor->is<physx::PxRigidDynamic>()->putToSleep();
		}
	}

	void PhysicsActor::AddCollider(BoxColliderComponent& collider, Entity entity)
	{
		myColliders.emplace_back(CreateRef<BoxColliderShape>(collider, *this, entity));
	}

	void PhysicsActor::AddCollider(SphereColliderComponent& collider, Entity entity)
	{
		myColliders.emplace_back(CreateRef<SphereColliderShape>(collider, *this, entity));
	}

	void PhysicsActor::AddCollider(CapsuleColliderComponent& collider, Entity entity)
	{
		myColliders.emplace_back(CreateRef<CapsuleColliderShape>(collider, *this, entity));
	}

	void PhysicsActor::AddCollider(MeshColliderComponent& collider, Entity entity)
	{
		if (collider.isConvex)
		{
			myColliders.emplace_back(CreateRef<ConvexMeshShape>(collider, *this, entity));
		}
		else
		{
			myColliders.emplace_back(CreateRef<TriangleMeshShape>(collider, *this, entity));
		}
	}

	void PhysicsActor::RemoveCollider(ColliderType type)
	{
		auto it = std::find_if(myColliders.begin(), myColliders.end(), [type](Ref<ColliderShape> coll) 
			{
				return coll->GetType() == type;
			});

		if (it != myColliders.end())
		{
			(*it)->DetachFromActor(myRigidActor);
			(*it)->Release();
			myColliders.erase(it);
		}
	}

	void PhysicsActor::SetLockFlag(ActorLockFlag flag, bool value, bool forceAwake)
	{
		if (!IsDynamic())
		{
			return;
		}

		if (value)
		{
			myLockFlags |= (uint32_t)flag;
		}
		else
		{
			myLockFlags &= ~(uint32_t)flag;
		}

		myRigidActor->is<physx::PxRigidDynamic>()->setRigidDynamicLockFlags((physx::PxRigidDynamicLockFlags)myLockFlags);
	}

	void PhysicsActor::CreateRigidActor()
	{
		auto& sdk = PhysXInternal::GetPhysXSDK();
		const gem::mat4 transform = myEntity.GetWorldTransform();
	
		if (myRigidBodyData.bodyType == BodyType::Static)
		{
			myRigidActor = sdk.createRigidStatic(PhysXUtilities::ToPhysXTransform(transform));
		}
		else
		{
			const PhysicsSettings& settings = Physics::GetSettings();
			myRigidActor = sdk.createRigidDynamic(PhysXUtilities::ToPhysXTransform(transform));

			SetLinearDrag(myRigidBodyData.linearDrag);
			SetAngularDrag(myRigidBodyData.angularDrag);
			SetKinematic(myRigidBodyData.isKinematic);
			SetGravityDisabled(myRigidBodyData.disableGravity);

			myRigidActor->is<physx::PxRigidDynamic>()->setSolverIterationCounts(settings.solverIterations, settings.solverVelocityIterations);
			myRigidActor->is<physx::PxRigidDynamic>()->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, myRigidBodyData.collisionType == CollisionDetectionType::Continuous);
			myRigidActor->is<physx::PxRigidDynamic>()->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_SPECULATIVE_CCD, myRigidBodyData.collisionType == CollisionDetectionType::ContinuousSpeculative);
		}

		if (myEntity.HasComponent<BoxColliderComponent>()) AddCollider(myEntity.GetComponent<BoxColliderComponent>(), myEntity);
		if (myEntity.HasComponent<SphereColliderComponent>()) AddCollider(myEntity.GetComponent<SphereColliderComponent>(), myEntity);
		if (myEntity.HasComponent<CapsuleColliderComponent>()) AddCollider(myEntity.GetComponent<CapsuleColliderComponent>(), myEntity);
		if (myEntity.HasComponent<MeshColliderComponent>()) AddCollider(myEntity.GetComponent<MeshColliderComponent>(), myEntity);

		SetMass(myRigidBodyData.mass);
		myRigidActor->userData = this;

		SetSimulationData(myRigidBodyData.layerId);
	}
	      
	void PhysicsActor::SynchronizeTransform()
	{
		if (myToBeRemoved)
		{
			return;
		}

		TransformComponent& transComp = myEntity.GetComponent<TransformComponent>();
		physx::PxTransform actorPose = myRigidActor->getGlobalPose();
		
		transComp.position = PhysXUtilities::FromPhysXVector(actorPose.p);
		if (!IsAllRotationLocked())
		{
			transComp.rotation = gem::eulerAngles(PhysXUtilities::FromPhysXQuat(actorPose.q));
		}
	}
}