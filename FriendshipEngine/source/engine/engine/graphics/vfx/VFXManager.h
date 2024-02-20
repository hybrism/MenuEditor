#pragma once
//#include <ShaderTypeEnum.h>
#include "VisualEffect.h"
#include <bitset>
#include <array>
#include <engine/math/Transform.h>

class Mesh;

struct VFXData
{
	std::bitset<MAX_EFFECT_SIZE> cellHasStarted = {};
	std::bitset<MAX_EFFECT_SIZE> cellHasCompleted = {};
	Transform worldTransform = {};
	int activeMeshes[MAX_EFFECT_SIZE];
	float activeMeshesTime[MAX_EFFECT_SIZE];
	size_t size = MAX_EFFECT_SIZE;
	vfxid_t visualEffectId = 0;
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

#define VFX_MANAGER_MAX_EFFECTS 1024

class VFXManager
{
public:
	VFXManager(VFXDatabase& aDatabase);
	~VFXManager();

	size_t InsertEffect(const vfxid_t& aData);
	void Play(const size_t& aIndex);
	void Stop(const size_t& aIndex);
	void Update(const float& dt);
	void Render();

private:
	std::array<VFXData, VFX_MANAGER_MAX_EFFECTS> myActiveVFX;
	size_t myActiveVFXCount;
	VFXDatabase* myVfxDatabase;
};
