#include "pch.h"
#include "AnimationPlayer.h"
#include "../model/SkeletalMesh.h"
#include "Animation.h"
#include "AnimationStructs.h"
#include "AnimationState.h"
#include <assets/AssetDatabase.h>

void AnimationPlayer::UpdatePose(
	const float& aDeltaTime,
	size_t aMeshId,
	AnimationData& aData,
	const AnimationState& aFromState,
	const AnimationState* aToState
)
{
	const SharedMeshPackage& meshPackage = AssetDatabase::GetMesh(aMeshId);

	Pose pose = InternalUpdateAndGetPose(aDeltaTime, aMeshId, 0, meshPackage, aData, aFromState);

	if (aToState != nullptr)
	{
		Pose toPose = InternalUpdateAndGetPose(aDeltaTime, aMeshId, 1, meshPackage, aData, *aToState);
		pose = GetInterpolatedPose(pose, toPose, *meshPackage.skeleton, aData.blendFactor);
	}

	aData.localSpacePose = pose;
}

void AnimationPlayer::Play(AnimationData& aData)
{
	myIsPlaying = true;
	aData.time[0] = 0;
	aData.time[1] = 0;
}

void AnimationPlayer::Pause()
{
	myIsPlaying = false;
}

void AnimationPlayer::Stop(AnimationData& aData)
{
	myIsPlaying = false;
	aData.time[0] = 0;
	aData.time[1] = 0;
}

unsigned int AnimationPlayer::GetFrame(AnimationData& aData, const Animation& aAnimation) const
{
	//return static_cast<unsigned int>(std::floor(myTime / (1.0f / myFPS)));
	return static_cast<unsigned int>(aData.time[0] * aAnimation.frameRate);
}

void AnimationPlayer::SetFrame(AnimationData& aData, const Animation& aAnimation, const unsigned int& aFrame)
{
	aData.time[0] = static_cast<float>(aFrame) / aAnimation.frameRate;
	aData.time[1] = 0.0f;
	aData.transitionIndex = -1;
	aData.nextStateIndex = aData.currentStateIndex;
}

#include <iostream>
Pose AnimationPlayer::InternalUpdateAndGetPose(
	const float& aDeltaTime,
	size_t aMeshId,
	size_t aTimeIndex,
	const SharedMeshPackage& aMeshPackage,
	AnimationData& aData,
	const AnimationState& aState
) const
{
	Animation* animation = AssetDatabase::GetAnimation(aMeshId, aState.GetAnimationIndex());
	Skeleton* skeleton = aMeshPackage.skeleton;

	float& time = aData.time[aTimeIndex];
	time += aDeltaTime * aData.speed;

	unsigned int count = animation->length - 1;
	const float frameRate = 1.0f / animation->frameRate;
	size_t currentFrame = static_cast<size_t>(std::floor(time / frameRate));
	size_t nextFrame = currentFrame + 1;

	if (time >= animation->duration)
	{
		if (aState.IsLooping())
		{
			float delta = time - animation->duration;
			time = std::fmodf(delta, animation->duration);
			currentFrame = static_cast<size_t>(std::floor(time / frameRate));
			nextFrame = currentFrame + 1;
		}
		else
		{
			time = animation->duration;
			currentFrame = count;
			nextFrame = currentFrame;
		}
	}

	float nextFrameTime = nextFrame * frameRate;
	float blendFactor = 1.0f - (nextFrameTime - time) / frameRate; // delta from current time and time of next frame

	if (nextFrame > count) {
		nextFrame = 0;
	}

	return GetInterpolatedPose(animation->frames[currentFrame], animation->frames[nextFrame], *skeleton, blendFactor);
}

Pose AnimationPlayer::GetInterpolatedPose(
	const Pose& aPoseA,
	const Pose& aPoseB,
	const Skeleton& aSkeleton,
	const float& aBlendFactor
) const
{
	Pose pose;

	pose.count = aSkeleton.bones.size();
	for (size_t i = 0; i < aSkeleton.bones.size(); i++)
	{
		const Transform& jointA = aPoseA.jointTransforms[aSkeleton.bones[i].id];

		DirectX::XMMATRIX jointXform = jointA.GetMatrix();
#ifdef SHOULD_INTERPOLATE_ANIMATIONS
		{
			const Transform& jointB = aPoseB.jointTransforms[aSkeleton.bones[i].id];
			DirectX::FXMVECTOR rotationA = DirectX::XMQuaternionRotationMatrix(jointA.GetMatrix());
			DirectX::FXMVECTOR rotationB = DirectX::XMQuaternionRotationMatrix(jointB.GetMatrix());

			const Vector3f			S = Vector3f::Lerp(jointA.GetScale(), jointB.GetScale(), aBlendFactor);
			const DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(rotationA, rotationB, aBlendFactor);
			const Vector3f			T = Vector3f::Lerp(jointA.GetPosition(), jointB.GetPosition(), aBlendFactor);
			jointXform = DirectX::XMMatrixScaling(S.x, S.y, S.z) * DirectX::XMMatrixRotationQuaternion(R) * DirectX::XMMatrixTranslation(T.x, T.y, T.z);
		}
#endif
		pose.jointTransforms[i] = jointXform;
	}

	return pose;
}
