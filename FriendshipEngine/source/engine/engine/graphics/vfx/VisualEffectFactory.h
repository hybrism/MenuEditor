#pragma once
#include "VisualEffect.h"

class VisualEffectFactory
{
public:
	static VisualEffectCell CreateVFXCell(
		PsType aPsType,
		int aMeshId,
		const Transform& aTransform,
		float aStartTime,
		float aDuration
	);

	static VisualEffectCell CreateEmitterCell(
		int aParticleIndex,
		const Transform& aTransform,
		float aStartTime,
		float aDuration
	);
};
