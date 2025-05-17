#pragma once
#include <assets/ShaderTypeEnum.h>
#include "../../math/Transform.h"

#define MAX_EFFECT_SIZE 4
typedef size_t vfxid_t;

struct ParticleEmitter
{
	float spawnRate = 0;
	float spawnRateVariation = 0;
	float spawnRadius = 0;
	float spawnAngle = 0;
	float spawnAngleVariation = 0;
	float speed = 0;
	float speedVariation = 0;
	float lifeSpan = 0;
	float lifeSpanVariation = 0;
	float size = 0;
	float sizeVariation = 0;
	float rotation = 0;
	float rotationVariation = 0;
	float gravity = 0;
	float gravityVariation = 0;
	float drag = 0;
	float dragVariation = 0;
	Vector4f color = { 1, 1, 1, 1 };
	Vector4f colorVariation = { 0, 0, 0, 0 };
	Vector3f position = { 0, 0, 0 };
	Vector3f velocity = { 0, 0, 0 };
	size_t maxParticles = 1024;
};

enum class VisualEffectType
{
	VFX,
	ParticleEmitter,
};

//struct VisualEffectData
//{
//	// id = meshId
//	VisualEffectBaseData base = { VisualEffectType::VFX, -1 };
//	PsType psType = PsType::Count;
//};
//
//struct ParticleEmitterData
//{
//	// id = particleEmitterIndex
//	VisualEffectBaseData base = { VisualEffectType::ParticleEmitter, -1 };
//};

//union VisualEffectTypeUnion
//{
//	VisualEffectData vfx;
//	ParticleEmitterData emitter;
//};

struct VisualEffectBaseData
{
	VisualEffectType type = VisualEffectType::VFX;
	PsType psType = PsType::Count;
	int id = -1;
};

struct VisualEffectCell
{
	Transform transform = {};
	//VisualEffectTypeUnion effectType = {};
	VisualEffectBaseData effectType = {};
	int meshId = -1;
	float startTime = 0;
	float duration = 0;
};

class VisualEffect
{
public:
	VisualEffect() = default;
	~VisualEffect() = default;

	void AddCell(VisualEffectCell aCell)
	{
		myEffects[mySize] = aCell;
		++mySize;
	}

	const VisualEffectCell& GetCell(size_t aEffectIndex) const
	{
		return myEffects[aEffectIndex];
	}

private:
	VisualEffectCell myEffects[MAX_EFFECT_SIZE];
	size_t mySize = 0;
};
