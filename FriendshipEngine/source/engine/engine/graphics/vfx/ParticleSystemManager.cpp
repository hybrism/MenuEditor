#include "pch.h"
#include "ParticleSystemManager.h"
#include "../GraphicsEngine.h"
#include "../renderer/ForwardRenderer.h"
#include <assets/AssetDatabase.h>

ParticleSystemManager::ParticleSystemManager()
{
}

ParticleSystemManager::~ParticleSystemManager()
{
}

size_t ParticleSystemManager::InsertEffect(const ParticleEmitter& aEmitter)
{
	myActiveEmitters.push_back({});
	myActiveEmitters.back().emitter = aEmitter;
	myActiveEmitters.back().particles.resize(aEmitter.maxParticles);
	return myActiveEmitters.size() - 1;
}

void ParticleSystemManager::RemoveEmitter(size_t aIndex)
{
	aIndex;

	myActiveEmitters[aIndex].shouldBeDestroyed = true;
}

void ParticleSystemManager::Play(const size_t& aIndex)
{
	aIndex;
}

void ParticleSystemManager::Stop(const size_t& aIndex)
{
	aIndex;
}

void ParticleSystemManager::Update(const float& dt)
{
	for (size_t activeEmitterIndex = 0; activeEmitterIndex < myActiveEmitters.size(); ++activeEmitterIndex)
	{
		auto& container = myActiveEmitters[activeEmitterIndex];

		if (!container.shouldBeDestroyed)
		{
			int amountToSpawnThisFrame = int(container.emitter.spawnRate * dt);
			if (container.activeParticles + amountToSpawnThisFrame > container.emitter.maxParticles)
			{
				amountToSpawnThisFrame = int(container.emitter.maxParticles - container.activeParticles);
			}

			if (amountToSpawnThisFrame > 0)
			{
				for (size_t i = 0; i < amountToSpawnThisFrame; ++i)
				{
					Particle& particle = container.particles[container.activeParticles];
					particle.position = container.emitter.position;

					Vector3f offset = Vector3f{
						container.emitter.spawnRadius * (rand() % 1000 / 1000.0f) * (rand() % 2 * 2 - 1),
						container.emitter.spawnRadius * (rand() % 1000 / 1000.0f) * (rand() % 2 * 2 - 1),
						container.emitter.spawnRadius * (rand() % 1000 / 1000.0f) * (rand() % 2 * 2 - 1)
					};
					particle.position += offset;


					particle.size = container.emitter.size *
						container.emitter.sizeVariation *
						static_cast<float>(rand() % 100) / 100.0f;
					

					particle.velocity = container.emitter.velocity;
					particle.lifeTime = 0;
					particle.lifeSpan = container.emitter.lifeSpan + container.emitter.lifeSpanVariation * (rand() % 1000 / 1000.0f);
					++container.activeParticles;
				}
			}
		}
		else if (container.activeParticles == 0)
		{
			myActiveEmitters.erase(myActiveEmitters.begin() + activeEmitterIndex);
		}

		for (size_t particleIndex = 0; particleIndex < container.activeParticles; ++particleIndex)
		{
			auto & particle = container.particles[particleIndex];

			particle.velocity += Vector3f {
					1.0f * (rand() % 1000 / 1000.0f) * (rand() % 2 * 2 - 1),
					1.0f * (rand() % 1000 / 1000.0f) * (rand() % 2 * 2 - 1),
					1.0f * (rand() % 1000 / 1000.0f) * (rand() % 2 * 2 - 1)
			} *dt;

			particle.position += particle.velocity * dt;
			particle.lifeTime += dt;
			if (particle.lifeTime > particle.lifeSpan)
			{
				particle.lifeTime = 0;
				particle.position = container.emitter.position;
				particle.velocity = container.emitter.velocity;
				--container.activeParticles;
				std::swap(particle, container.particles[container.activeParticles]);
			}
		}
	}
}

void ParticleSystemManager::Render()
{
	auto& forwardRenderer = GraphicsEngine::GetInstance()->GetForwardRenderer();

	Mesh* quadMesh = (Mesh*)AssetDatabase::GetMesh((size_t)PrimitiveMeshID::Quad).meshData[0];

	for (size_t i = 0; i <	myActiveEmitters.size(); i++)
	{
		Transform transform;
		transform.SetPosition(myActiveEmitters[i].emitter.position);
		transform.SetScale({ 
			myActiveEmitters[i].emitter.size,
			myActiveEmitters[i].emitter.size,
			myActiveEmitters[i].emitter.size
		});

		MeshInstanceRenderData renderData = {
			MeshDrawerInstanceData(transform, ParticleInstanceData()),
			VsType::Particle,
			PsType::Particle,
			RenderMode::TRIANGLELIST,
			transform
		};

		for (size_t j = 0; j < myActiveEmitters[i].activeParticles; j++)
		{
			transform.SetScale({ 
				myActiveEmitters[i].particles[j].size,
				myActiveEmitters[i].particles[j].size,
				myActiveEmitters[i].particles[j].size
			});
			transform.SetPosition(myActiveEmitters[i].particles[j].position);
			renderData.data.transform = transform;
			renderData.data.particleData = ParticleInstanceData{ 
				myActiveEmitters[i].emitter.color,
				myActiveEmitters[i].particles[j].lifeTime / myActiveEmitters[i].particles[j].lifeSpan
			};

			forwardRenderer.Render(quadMesh, renderData, BlendState::AlphaBlend);
		}
	}
}
