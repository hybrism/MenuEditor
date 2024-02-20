#pragma once
#include <memory>
#include "Pose.h"

class SkeletalMesh;
class AnimationState;
struct Animation;
struct AnimationData;

#define SHOULD_INTERPOLATE_ANIMATIONS

class AnimationPlayer
{
public:
	void Init(const Animation* aAnimation, const SkeletalMesh* aModel, AnimationData* aData);
	void Update(const float& aDeltaTime, const AnimationState& aAnimationState);
	void UpdatePose();

	void Play();
	void Pause();
	void Stop();

	void SetFrame(const unsigned int& aFrame);
	unsigned int GetFrame() const;
	bool IsValid() const;
private:
	const SkeletalMesh* myModel = nullptr;
	const Animation* myAnimation = nullptr;
	AnimationData* myData = nullptr;

	bool myIsPlaying = false;
};
