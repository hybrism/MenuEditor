#include "pch.h"
#include "AnimatorSystem.h"
#include "../component/MeshComponent.h"
#include <ecs/World.h>
#include <assets/ModelFactory.h>
#include <iostream>

#include <assets/AssetDatabase.h>
#include <engine/graphics/model/SkeletalMesh.h>
#include <engine/graphics/animation/AnimationPlayer.h>
#include "..\component\TransformComponent.h"
#include "..\component\AnimationDataComponent.h"
#include "..\component\MeshComponent.h"
#include <engine/graphics\animation\AnimationController.h>

AnimatorSystem::AnimatorSystem(World* aWorld) : System(aWorld)
{
	myAnimationPlayer = new AnimationPlayer();
}

AnimatorSystem::~AnimatorSystem()
{
	delete myAnimationPlayer;
}

void AnimatorSystem::Init()
{
	for (auto& entity : myEntities)
	{
		auto& animData = myWorld->GetComponent<AnimationDataComponent>(entity);
		size_t meshId = myWorld->GetComponent<MeshComponent>(entity).id;
		Skeleton* skeleton = AssetDatabase::GetSkeleton(meshId);

		if (!AssetDatabase::DoesAnimationControllerExist(skeleton)) { continue; }

		auto& controller = AssetDatabase::GetAnimationController(animData.controllerId);

		animData.currentStateIndex = controller.GetDefaultStateIndex();
	}
}

// TODO: Add time, transitions etc to animation data component and use that instead of multiple animation players
void AnimatorSystem::Update(SceneUpdateContext& aContext)
{
	for (auto& entity : myEntities)
	{
		auto& animData = myWorld->GetComponent<AnimationDataComponent>(entity);
		size_t meshId = myWorld->GetComponent<MeshComponent>(entity).id;

		if (animData.controllerId < 0) { continue; }

		auto& controller = AssetDatabase::GetAnimationController(animData.controllerId);

		// Prevents crash if no animation states are added to skinned mesh, though this should never happen
#ifndef _RELEASE
		if (!controller.HasAnimationStates())
		{
			continue;
		}
#endif
		int currentAnimIndex = controller.GetCurrentAnimationIndex(animData);
		
		if (currentAnimIndex < 0) { continue; }

		Animation* animation = AssetDatabase::GetAnimation(AssetDatabase::GetSkeleton(meshId), currentAnimIndex);

		controller.Update(aContext.dt, *animation, animData);

		AnimationState* toState = nullptr;
		if (animData.IsTransitioning())
		{
			toState = &controller.GetState(animData.nextStateIndex);
		}

		myAnimationPlayer->UpdatePose(aContext.dt, meshId, animData, controller.GetCurrentState(animData), toState);
	}
}
