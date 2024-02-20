#pragma once
#include <assets/ShaderTypeEnum.h>
#include "../../math/Transform.h"

#define MAX_EFFECT_SIZE 4
typedef size_t vfxid_t;

struct VisualEffectCell
{
	Transform transform = {};
	PsType type = PsType::Count;
	size_t meshId = 0;
	float startTime = 0;
	float duration = 0;
};

class VisualEffect
{
public:
	VisualEffect() = default;
	~VisualEffect() = default;

	void Spawn(size_t aEffectIndex)
	{
		VisualEffectCell& effect = myEffects[aEffectIndex];
		effect;
	}

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
