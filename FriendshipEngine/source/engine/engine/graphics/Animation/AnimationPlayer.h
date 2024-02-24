#pragma once
#include <memory>
#include "Pose.h"

class SkeletalMesh;
class AnimationState;
struct Animation;
struct Skeleton;
struct AnimationData;
struct SharedMeshPackage;

#define SHOULD_INTERPOLATE_ANIMATIONS

class AnimationPlayer
{
public:
	void UpdatePose(
		const float& aDeltaTime,
		size_t aMeshId,
		AnimationData& aData,
		const AnimationState& aFromState,
		const AnimationState* aToState = nullptr
	);
	//void UpdatePose(AnimationData& aData);

	void Play(AnimationData& aData);
	void Pause();
	void Stop(AnimationData& aData);

	void SetFrame(AnimationData& aData, const Animation& aAnimation, const unsigned int& aFrame);
	unsigned int GetFrame(AnimationData& aData, const Animation& aAnimation) const;
private:
	Pose InternalUpdateAndGetPose(
		const float& aDeltaTime,
		size_t aMeshId,
		size_t aTimeIndex,
		const SharedMeshPackage& aMeshPackage,
		AnimationData& aData,
		const AnimationState& aState
	) const;

	Pose GetInterpolatedPose(
		const Pose& aPoseA,
		const Pose& aPoseB,
		const Skeleton& aSkeleton,
		const float& aBlendFactor
	) const;

	bool myIsPlaying = false;
};
