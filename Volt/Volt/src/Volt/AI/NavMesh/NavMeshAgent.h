#pragma once
#include <gem/gem.h>

#include <stack>

namespace Volt
{
	struct TransformComponent;

	class NavMeshAgent
	{
	public:
		NavMeshAgent();
		~NavMeshAgent();

		void StartNavigation();
		void StopNavigation();
		void SetTarget(gem::vec3 aPosition);

		void SetSpeed(const float& aSpeed) { mySpeed = aSpeed; };
		float GetSpeed() const { return mySpeed; }

	private:
		void Update(float aTimestep, TransformComponent& aTransformComp);
		void MoveToTarget(float aTimestep, TransformComponent& aTransformComp);
		std::stack<gem::vec3> FindPath(const gem::vec3& start, const gem::vec3& target);

	private:
		friend class NavigationsSystem;

		float mySpeed = 100.f;

		gem::vec3 myCurrent = { 0.f, 0.f, 0.f };
		gem::vec3 myTarget = { 0.f, 0.f, 0.f };
		std::stack<gem::vec3> myPath;

		bool myActive = true;
	};
}