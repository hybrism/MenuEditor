#pragma once
#include "VisualEffect.h"

#include <vector>

struct Particle
{
	Vector3f position;
	Vector3f velocity;
	float lifeTime;
	float lifeSpan;
	float size;
};

struct ParticleEmitterContainer
{
	ParticleEmitter emitter;
	std::vector<Particle> particles;
	size_t activeParticles = 0;
	bool shouldBeDestroyed = false;
};

class ParticleSystemManager
{
public:
	ParticleSystemManager();
	~ParticleSystemManager();

	size_t InsertEffect(const ParticleEmitter& aEmitter);

	void RemoveEmitter(size_t aIndex);

	void Play(const size_t& aIndex);

	void Stop(const size_t& aIndex);

	void Update(const float& dt);
	void Render();

	void Reset() { myActiveEmitters.clear(); }
private:
	std::vector<ParticleEmitterContainer> myActiveEmitters;
	
};
