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

		if (!AssetDatabase::DoesAnimationControllerExist(meshId)) { continue; }

		auto* controller = AssetDatabase::GetAnimationController(meshId);

		animData.currentStateIndex = controller->GetDefaultStateIndex();
	}
}

// TODO: Add time, transitions etc to animation data component and use that instead of multiple animation players
void AnimatorSystem::Update(const float& dt)
{
	for (auto& entity : myEntities)
	{
		auto& animData = myWorld->GetComponent<AnimationDataComponent>(entity);
		size_t meshId = myWorld->GetComponent<MeshComponent>(entity).id;

		if (!AssetDatabase::DoesAnimationControllerExist(meshId)) { continue; }

		auto* controller = AssetDatabase::GetAnimationController(meshId);

		// Prevents crash if no animation states are added to skinned mesh, though this should never happen
#ifdef _DEBUG
		if (!controller->HasAnimationStates())
		{
			continue;
		}
#endif
		int currentAnimIndex = controller->GetCurrentAnimationIndex(animData);
		
		if (currentAnimIndex < 0) { continue; }

		Animation* animation = AssetDatabase::GetAnimation(meshId, currentAnimIndex);

		controller->Update(dt, *animation, animData);

		AnimationState* toState = nullptr;
		if (animData.IsTransitioning())
		{
			toState = &controller->GetState(animData.nextStateIndex);
		}

		myAnimationPlayer->UpdatePose(dt, meshId, animData, controller->GetCurrentState(animData), toState);
	}
}
