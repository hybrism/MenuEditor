#pragma once
//#include <ShaderTypeEnum.h>
#include "VisualEffect.h"
#include <bitset>
#include <vector>
#include <engine/math/Transform.h>

class Mesh;

struct VFXData
{
	Transform worldTransform = {};

	float activeMeshesTime[MAX_EFFECT_SIZE];
	int activeMeshes[MAX_EFFECT_SIZE];

	size_t size = MAX_EFFECT_SIZE;
	vfxid_t visualEffectId = 0;

	std::bitset<MAX_EFFECT_SIZE> cellHasStarted = {};
	std::bitset<MAX_EFFECT_SIZE> cellHasCompleted = {};

	float time = 0;
	bool isPlaying = false;

	bool IsComplete() const
	{
		return cellHasCompleted.all();
	}

	void Reset()
	{
		cellHasStarted.reset();
		cellHasCompleted.reset();
		ZeroMemory(&activeMeshes, sizeof(activeMeshes));
		ZeroMemory(&activeMeshesTime, sizeof(activeMeshesTime));
		time = 0;
		isPlaying = false;
	}
};

class VFXDatabase;
class ParticleSystemManager;

#define VFX_MANAGER_DEFAULT_EFFECTS_SIZE 32
class VFXManager
{
public:
	VFXManager(ParticleSystemManager& aParticleSystemManager);
	~VFXManager();

	size_t InsertEffect(const vfxid_t& aData);
	void Play(const size_t& aIndex);
	void Stop(const size_t& aIndex);
	void Update(const float& dt);
	void Render();

private:
	std::vector<VFXData> myActiveVFX;
	size_t myActiveVFXCount;
	VFXDatabase* myVfxDatabase;
	ParticleSystemManager* myParticleSystemManager;
};
