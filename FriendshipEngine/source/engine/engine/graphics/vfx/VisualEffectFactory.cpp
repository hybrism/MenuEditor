#include "pch.h"
#include "VisualEffectFactory.h"
#include <assets/AssetDatabase.h>

VisualEffectCell VisualEffectFactory::CreateVFXCell(PsType aPsType, int aMeshId, const Transform& aTransform, float aStartTime, float aDuration)
{
	VisualEffectCell cell;
	cell.transform = aTransform;
	cell.startTime = aStartTime;
	cell.duration = aDuration;
	cell.meshId = aMeshId;
	cell.effectType.type = VisualEffectType::VFX;
	cell.effectType.psType = aPsType;
	cell.effectType.id = -1;
	
	return cell;
}

VisualEffectCell VisualEffectFactory::CreateEmitterCell(int aParticleIndex, const Transform& aTransform, float aStartTime, float aDuration)
{
	VisualEffectCell cell;
	cell.transform = aTransform;
	cell.startTime = aStartTime;
	cell.duration = aDuration;
	cell.meshId = static_cast<int>(PrimitiveMeshID::Quad);
	cell.effectType.type = VisualEffectType::ParticleEmitter;
	cell.effectType.psType = PsType::Particle;
	cell.effectType.id = aParticleIndex;
	
	return cell;
}
