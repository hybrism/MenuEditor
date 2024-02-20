#include "pch.h"
#include "AnimationPlayer.h"
#include "../model/SkeletalMesh.h"
#include "Animation.h"
#include "AnimationStructs.h"
#include "AnimationState.h"

void AnimationPlayer::Init(const Animation* aAnimation, const SkeletalMesh* aModel, AnimationData* aData)
{
	myAnimation = aAnimation;
	myModel = aModel;
	myData = aData;
}

void AnimationPlayer::Update(const float& dt, const AnimationState& aAnimationState)
{
	const float frameRate = 1.0f / myAnimation->frameRate;
	const float result = myData->time / frameRate;
#ifdef _DEBUG // this should only occur if a breakpoint is active
	size_t currentFrame = static_cast<size_t>(std::floor(result));
	if (currentFrame > myAnimation->length) { currentFrame = 0; }
#else
	const size_t currentFrame = static_cast<size_t>(std::floor(result));
#endif
	const float delta = result - static_cast<float>(currentFrame);
	size_t nextFrame = currentFrame + 1;

	if (nextFrame > myAnimation->length) { nextFrame = 0; }

	myData->time += dt;

	if (myData->time >= myAnimation->duration)
	{
		if (aAnimationState.IsLooping())
		{
			// TODO: Optimize this
			while (myData->time >= myAnimation->duration)
				myData->time -= myAnimation->duration;
		}
		else
		{
			myData->time = myAnimation->duration;
			nextFrame = currentFrame;
		}
	}

	// Update all animations
	const Skeleton& skeleton = myModel->GetSkeleton();
	for (size_t i = 0; i < skeleton.bones.size(); i++)
	{
		// nåt går jättefel om den hoppar in i detta if statement D:
		if (myAnimation->frames[currentFrame].localTransforms.find(skeleton.bones[i].id) == myAnimation->frames[currentFrame].localTransforms.end()) { continue; }

		const Transform& currentFrameJointXform = myAnimation->frames[currentFrame].localTransforms.at(skeleton.bones[i].id);
		//const Transform& currentFrameJointXform = myAnimation->Frames[currentFrame].LocalTransforms.find(skeleton.bones[i].name)._Ptr->_Myval.second;
		DirectX::XMMATRIX jointXform = currentFrameJointXform.GetMatrix();
#ifdef SHOULD_INTERPOLATE_ANIMATIONS
		{
			const Transform& nextFrameJointXform = myAnimation->frames[nextFrame].localTransforms.at(skeleton.bones[i].id);
			DirectX::FXMVECTOR currentFrameRotation = DirectX::XMQuaternionRotationMatrix(currentFrameJointXform.GetMatrix());
			DirectX::FXMVECTOR nextFrameRotation = DirectX::XMQuaternionRotationMatrix(nextFrameJointXform.GetMatrix());

			// Interpolate between the frames
			const Vector3f T = Vector3f::Lerp(currentFrameJointXform.GetPosition(), nextFrameJointXform.GetPosition(), delta);
			const  DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(currentFrameRotation, nextFrameRotation, delta);
			const Vector3f S = Vector3f::Lerp(currentFrameJointXform.GetScale(), nextFrameJointXform.GetScale(), delta);
			jointXform = DirectX::XMMatrixScaling(S.x, S.y, S.z) * DirectX::XMMatrixRotationQuaternion(R) * DirectX::XMMatrixTranslation(T.x, T.y, T.z);
		}
#endif
		myData->localSpacePose.jointTransforms[i] = jointXform;
	}
	myData->localSpacePose.count = skeleton.bones.size();
}

void AnimationPlayer::UpdatePose()
{
	const size_t frame = GetFrame();// Which frame we're on
	// Update all animations
	const Skeleton& skeleton = myModel->GetSkeleton();
	for (size_t i = 0; i < skeleton.bones.size(); i++)
	{
		const Transform& currentFrameJointXform = myAnimation->frames[frame].localTransforms.at(skeleton.bones[i].id);
		myData->localSpacePose.jointTransforms[i] = currentFrameJointXform.GetMatrix();
	}
	myData->localSpacePose.count = skeleton.bones.size();
}

void AnimationPlayer::Play()
{
	myIsPlaying = true;
	myData->time = 0;
}

void AnimationPlayer::Pause()
{
	myIsPlaying = false;
}

void AnimationPlayer::Stop()
{
	myIsPlaying = false;
	myData->time = 0;
}

unsigned int AnimationPlayer::GetFrame() const
{
	//return static_cast<unsigned int>(std::floor(myTime / (1.0f / myFPS)));
	return static_cast<unsigned int>(myData->time * myAnimation->frameRate);
}

void AnimationPlayer::SetFrame(const unsigned int& aFrame)
{
	myData->time = static_cast<float>(aFrame) / myAnimation->frameRate;
}

bool AnimationPlayer::IsValid() const
{
	return myAnimation != nullptr;
}
