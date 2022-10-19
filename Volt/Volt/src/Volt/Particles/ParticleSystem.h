#pragma once
#include "../Components/Components.h"

namespace Volt
{
class Scene;

	class ParticleSystem {
	public:
		ParticleSystem(Scene*);
		void Update(const float& deltaTime);
		void RenderParticles();
	private:
		void SendParticles(ParticleEmitterComponent& particleEmitterComponent, TransformComponent& transformComp, const float& deltaTime);
		bool ParticleKillCheck(Particle& particle);
		void ParticlePositionUpdate(Particle& particle, const float& deltaTime);
		Scene* myScene;
	};
}