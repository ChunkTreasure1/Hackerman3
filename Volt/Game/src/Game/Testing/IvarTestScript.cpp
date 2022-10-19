#include "IvarTestScript.h"

#include <Volt/Components/PhysicsComponents.h>
#include <Volt/Components/Components.h>

#include <Volt/Asset/AssetManager.h>
#include <Volt/Input/KeyCodes.h>

#include <Volt/Physics/PhysicsActor.h>
#include <Volt/Physics/PhysicsScene.h>
#include <Volt/Physics/Physics.h>

#include <Volt/Utility/Random.h>

IvarTestScript::IvarTestScript(Volt::Entity entity)
	: Volt::ScriptBase(entity)
{}

void IvarTestScript::OnAwake()
{
}

void IvarTestScript::OnEvent(Volt::Event& e)
{
	Volt::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Volt::KeyPressedEvent>(VT_BIND_EVENT_FN(IvarTestScript::OnKeyPressedEvent));
}

void IvarTestScript::OnUpdate(float aDeltaTime)
{}

void IvarTestScript::OnCollisionEnter(Volt::Entity entity)
{
	VT_CORE_ASSERT(false);
}

bool IvarTestScript::OnKeyPressedEvent(Volt::KeyPressedEvent& e)
{
	switch (e.GetKeyCode())
	{
		case VT_KEY_K:
		{
			auto entity = myEntity.GetScene()->InstantiateSplitMesh("Assets/Meshes/ShatterTest/SM_ShatterTest.vtmesh");
			auto& relComp = entity.GetComponent<Volt::RelationshipComponent>();

			entity.SetPosition({ 0.f, 150.f, 0.f });

			for (const auto& child : relComp.Children)
			{
				Volt::Entity childEnt = Volt::Entity{ child, myEntity.GetScene() };

				auto& meshCollComp = childEnt.AddComponent<Volt::MeshColliderComponent>();
				meshCollComp.colliderMesh = childEnt.GetComponent<Volt::MeshComponent>().handle;
				meshCollComp.subMeshIndex = childEnt.GetComponent<Volt::MeshComponent>().subMeshIndex;
				meshCollComp.isConvex = true;

				Volt::RigidbodyComponent rigidInit{};
				rigidInit.bodyType = Volt::BodyType::Dynamic;
				auto& rigidComp = childEnt.AddComponent<Volt::RigidbodyComponent>(&rigidInit);

				auto actor = Volt::Physics::GetScene()->GetActor(childEnt);
				actor->AddForce({ Volt::Random::Float(-100, 1000), Volt::Random::Float(40, 1000), Volt::Random::Float(-1000, 1000) }, Volt::ForceMode::Impulse);
			}

			auto children = relComp.Children;
			for (const auto& child : children)
			{
				Volt::Entity childEnt = Volt::Entity{ child, myEntity.GetScene() };
				myEntity.GetScene()->UnparentEntity(childEnt);
			}

			break;
		}
	}

	return false;
}

VT_REGISTER_SCRIPT(IvarTestScript);