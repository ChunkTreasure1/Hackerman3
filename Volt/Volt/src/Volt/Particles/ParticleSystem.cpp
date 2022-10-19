#include "vtpch.h"
#include "ParticleSystem.h"

#include "GEM/gem.h"
#include "../Scene/Scene.h"
#include "Volt/Rendering/Renderer.h"

#include "Volt/Utility/SerializationMacros.h"
#include "Volt/Utility/YAMLSerializationHelpers.h"
#include "Volt/Utility/FileSystem.h"

#include <Volt/Core/Base.h>
#include "Volt/Asset/AssetManager.h"
#include "Volt/Asset/ParticlePreset.h"
#include "Volt/Rendering/Shader/ShaderRegistry.h"
#include "Volt/Rendering/Shader/Shader.h"
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <filesystem>
#include <random>

Volt::ParticleSystem::ParticleSystem(Scene* s)
{
	myScene = s;
}

void Volt::ParticleSystem::Update(const float& deltaTime)
{
	auto& registry = myScene->GetRegistry();
	registry.ForEach<ParticleEmitterComponent, TransformComponent>([&](Wire::EntityId id, ParticleEmitterComponent& particleEmitterComponent, TransformComponent& transformComp)
		{
			if (particleEmitterComponent.preset == Asset::Null())
			{
				particleEmitterComponent.particles.resize(0);
				particleEmitterComponent.emittionTimer = 0;
				particleEmitterComponent.numberOfAliveParticles = 0;
				return;
			}
			Ref<ParticlePreset> preset = AssetManager::GetAsset<ParticlePreset>(particleEmitterComponent.preset);
			if (preset != nullptr)
			{
				if (particleEmitterComponent.preset != particleEmitterComponent.currentPreset)
				{
					particleEmitterComponent.currentPreset = particleEmitterComponent.preset;

					for (auto& p : particleEmitterComponent.particles)
						p.dead = true;

					particleEmitterComponent.particles.resize(preset->poolSize);
					particleEmitterComponent.emittionTimer = 0;
					particleEmitterComponent.numberOfAliveParticles = 0;

				}
				if (transformComp.visible)
				{
					particleEmitterComponent.emittionTimer += deltaTime;
					SendParticles(particleEmitterComponent, transformComp, deltaTime);
					auto& p_vec = particleEmitterComponent.particles;
					for (int index = 0; index < particleEmitterComponent.numberOfAliveParticles; index++)
					{
						// TODO: add updating stuff
						auto& p = p_vec[index];
						ParticlePositionUpdate(p, deltaTime);
						if (ParticleKillCheck(p))
						{
							std::swap(p, p_vec[particleEmitterComponent.numberOfAliveParticles - 1]);
							// -- ref
							particleEmitterComponent.numberOfAliveParticles--;
						}
					}
				}
			}
		}
	);
}

void Volt::ParticleSystem::RenderParticles()
{
	VT_PROFILE_FUNCTION();

	auto& registry = myScene->GetRegistry();
	registry.ForEach<ParticleEmitterComponent, TransformComponent>([&](Wire::EntityId id, ParticleEmitterComponent& particleEmitterComponent, const TransformComponent& transformComp)
		{
			if (transformComp.visible)
			{
				for (int i = 0; i < particleEmitterComponent.numberOfAliveParticles; i++)
				{
					auto& p = particleEmitterComponent.particles[i];
					Renderer::SubmitBillboard(p.position, transformComp.scale * 0.001f, p.color, id);
				}
			}
			if (particleEmitterComponent.preset != Asset::Null())
			{
				Ref<ParticlePreset> preset = AssetManager::GetAsset<ParticlePreset>(particleEmitterComponent.preset);
				if (preset && preset->IsValid())
				{
					Ref<Volt::Shader> shader = ShaderRegistry::Get(preset->shader);
					if (shader != nullptr && shader->IsValid())
						Renderer::DispatchBillboardsWithShader(shader);
				}
			}
		});
}

void Volt::ParticleSystem::SendParticles(ParticleEmitterComponent& particleEmitterComponent, TransformComponent& transformComp, const float& deltaTime)
{
	auto e = AssetManager::GetAsset<ParticlePreset>(particleEmitterComponent.preset);
	while (particleEmitterComponent.emittionTimer > 1.0f / e->intensity
		&& particleEmitterComponent.numberOfAliveParticles < particleEmitterComponent.particles.size())
	{
		auto& p = particleEmitterComponent.particles[particleEmitterComponent.numberOfAliveParticles];

		// TODO: make different starting patterns <func>
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution distrib(-e->spread, e->spread);
		gem::vec3 spread;

		spread = { distrib(gen), distrib(gen), distrib(gen) }; spread += e->direction;
		while (spread.x == 0 && spread.y == 0 && spread.z == 0)
		{
			spread = { distrib(gen), distrib(gen), distrib(gen) }; spread += e->direction;
		}

		p.startVelocity = e->velocity;
		p.velocity = p.startVelocity;
		p.emissiveness = e->emissiveness;
		p.fade = e->fade;
		p.dead = false;
		p.direction = gem::normalize(spread);
		p.distance = 0;
		p.endDistance = e->distance;
		// temp	value  <--------->
		p.startColor = e->color;
		p.color = p.startColor;
		p.position = transformComp.position;
		p.gravity = e->gravity;
		particleEmitterComponent.numberOfAliveParticles++;
		particleEmitterComponent.emittionTimer -= 1.0f / e->intensity;
	}
}

bool Volt::ParticleSystem::ParticleKillCheck(Particle& particle)
{
	if (particle.distance > particle.endDistance && particle.endDistance > 0)
		particle.dead = true;
	return particle.dead;
}

void Volt::ParticleSystem::ParticlePositionUpdate(Particle& particle, const float& deltaTime)
{
	particle.position += particle.velocity * particle.direction * deltaTime;
	particle.distance += particle.velocity * deltaTime;
	particle.direction += particle.gravity * deltaTime;
	if (particle.fade)
		particle.color = { particle.color.x, particle.color.y, particle.color.z, particle.startColor.w-(particle.distance / particle.endDistance)};
}

