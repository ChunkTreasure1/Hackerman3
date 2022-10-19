#pragma once

#include <Wire/WireGUID.h>

#include "Volt/Scene/Entity.h"

namespace Volt
{
	class Entity;
	class ScriptBase
	{
	public:
		ScriptBase(Entity entity);
		virtual ~ScriptBase() = default;

		// This is called when the script is added to an entity
		virtual void OnAwake() {}

		// This is called when the script is removed from an entity
		virtual void OnDetach() {}

		// This is called when the scene starts playing
		virtual void OnStart() {}

		// This is called when the scene stops playing
		virtual void OnStop() {}

		// This is called every scene update
		virtual void OnUpdate(float aDeltaTime) {}

		// This is called every physics update
		virtual void OnFixedUpdate(float aDeltaTime) {}

		// This is called on every event
		virtual void OnEvent(Event& e) {}

		virtual void OnCollisionEnter(Entity entity) {}
		virtual void OnCollisionExit(Entity entity) {}

		virtual void OnTriggerEnter(Entity entity, bool isTrigger) {}
		virtual void OnTriggerExit(Entity entity, bool isTrigger) {}

		virtual WireGUID GetGUID() = 0;

	protected:
		Entity myEntity;
	};
}