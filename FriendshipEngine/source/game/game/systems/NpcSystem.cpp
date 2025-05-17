#include "pch.h"
#include "NpcSystem.h"
#include <ecs/World.h>
#include "../component/NpcComponent.h"
#include "../component/AnimationDataComponent.h"
#include "../component/TransformComponent.h"


#include <engine\utility\InputManager.h>
#include "utility\GameHelperFunctions.h"

#include <iostream>
#include <engine\graphics\Animation\AnimationController.h>
#include <assets\AssetDatabase.h>
#include <random>

NpcSystem::NpcSystem(World* aWorld) : System(aWorld)
{
}

void NpcSystem::Init()
{
	for (Entity entity : myEntities)
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dis(0.f, 1.f );

		auto& npcComponent = myWorld->GetComponent<NpcComponent>(entity);
	//	auto& transform = myWorld->GetComponent<TransformComponent>(entity);

		//transform.transform.SetPosition(myPos);
		//myPlayerPos = playerTransform.transform.GetPosition();
		
		npcComponent.DelayTimer = dis(gen);
		npcComponent.lerpingPos = npcComponent.StartPos;


	}

}

void NpcSystem::Update(SceneUpdateContext& aContext)
{


	for (Entity entity : myEntities)
	{
		auto& npcComponent = myWorld->GetComponent<NpcComponent>(entity);
		auto& transformComponent = myWorld->GetComponent<TransformComponent>(entity);
		auto& anim = myWorld->GetComponent<AnimationDataComponent>(entity);



		AnimationDataParameter parameter;

		auto& controller = AssetDatabase::GetAnimationController(myWorld->GetComponent<AnimationDataComponent>(entity).controllerId);

		parameter = static_cast<int>(npcComponent.myNpcState);

		controller.SetParameter("state", parameter, anim, anim.parameters);



		npcComponent.DelayTimer -= aContext.dt;

		npcComponent.myNpcState = npcState::walk;
		Vector3f dir = (npcComponent.walkToPos - npcComponent.lerpingPos).GetNormalized();
		Vector3f movement = dir * npcComponent.movementSpeed * aContext.dt;
		npcComponent.lerpingPos += movement;

		if ((npcComponent.walkToPos - npcComponent.lerpingPos).LengthSqr() <= (movement.LengthSqr()))
		{

			npcComponent.lerpingPos = npcComponent.walkToPos;
			std::swap(npcComponent.walkToPos, npcComponent.StartPos);

		}


		transformComponent.transform.SetPosition(npcComponent.lerpingPos);



		if (dir != Vector3f(0, 0, 0))
		{
			float yaw = atan2f(dir.x, dir.z) * 180.0f / M_FRIEND_PI;

			transformComponent.transform.SetEulerAngles(Vector3f(0, yaw, 0));

		}

	
	}

	
}


void NpcSystem::ActivateGroup(int aGroupID)
{
	for (Entity entity : myEntities)
	{
		auto& npcComponent = myWorld->GetComponent<NpcComponent>(entity);
		if (npcComponent.GroupID == aGroupID)
		{
			npcComponent.IsActive = true;
		}
	}
}
