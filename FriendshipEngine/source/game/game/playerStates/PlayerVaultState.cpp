#include "pch.h"
#include "PlayerVaultState.h"
#include "PlayerStateMachine.h"
#include "../component/PlayerComponent.h"
#include <physics/PhysXSceneManager.h>
#include <engine/graphics/Animation/AnimationController.h>
#include "../component/AnimationDataComponent.h"

PlayerVaultState::PlayerVaultState(PlayerStateMachine* aStateMachine) : PlayerState(aStateMachine)
{
}

void PlayerVaultState::OnEnter(PlayerStateUpdateContext& aContext)
{
	PlayerComponent& p = aContext.playerComponent;
	p.currentCameraHeight = PlayerConstants::cameraHeight;
	p.controller->resize(CHARACTER_HEIGHT);
	//float x = static_cast<float>(p.controller->getFootPosition().x);
	//x;
	Vector3f playerFootPos = { (float)p.controller->getFootPosition().x,(float)p.controller->getFootPosition().y, (float)p.controller->getFootPosition().z };
	float vaultHeight = abs(Vector3f::Distance(p.vaultRayCastOrigin, p.vaultLocation));
	PrintI("Vault Length: " + std::to_string(vaultHeight));
	float animationSpeed = 0;
	if (vaultHeight < 10.f)
	{
		PrintI("SLOW VAULT!");
		p.vaultDuration = 0.5f;
		p.savedVaultXVelocity = p.xVelocity * 0.1f;
		animationSpeed = 1.45f;
	}
	else if (10.f <= vaultHeight && vaultHeight < 35.f)
	{
		PrintI("MEDIUM VAULT!");
		p.vaultDuration = 0.3f;
		p.savedVaultXVelocity = p.xVelocity * 0.6f;
		animationSpeed = 1.85f;
	}
	else
	{
		PrintI("FAST VAULT!");
		p.vaultDuration = 0.1f;
		p.savedVaultXVelocity = p.xVelocity;
		animationSpeed = 3.0f;
	}

	p.timer.vaultTimer = 0;
	p.savedVaultXVelocity = p.xVelocity;

	p.xVelocity = { 0.f,0.f };
	p.yVelocity = 0.f;
	
	p.isVaulting = true;
	p.startVaultPosition = playerFootPos;
	p.endVaultPosition = p.vaultLocation;


	aContext.animationController.SetState((unsigned int)PlayerAnimationState::eVault, aContext.animationDataComponent);
	aContext.animationController.GetState((unsigned int)PlayerAnimationState::eVault).SetLooping(false);

	aContext.animationDataComponent.speed = animationSpeed;
	aContext.animationDataComponent.exitTimer = p.vaultDuration;
	/*auto anim = aContext.animationController->GetState((unsigned int)PlayerAnimationState::eVault);
	anim.SetSpeed(animationSpeed);*/

}

void PlayerVaultState::OnExit(PlayerStateUpdateContext& aContext)
{
	PlayerComponent& p = aContext.playerComponent;
	p.xVelocity = p.savedVaultXVelocity;
	p.yVelocity = 0.f;
	p.finalVelocity = { p.xVelocity.x, p.yVelocity, p.xVelocity.y };

}

void PlayerVaultState::Update(PlayerStateUpdateContext& aContext)
{
	PlayerComponent& p = aContext.playerComponent;
	const float dt = aContext.deltaTime;
	p.timer.vaultTimer = std::min(p.vaultDuration, p.timer.vaultTimer + dt);

	Vector3f currentPos = Vector3f::Lerp(p.startVaultPosition, p.endVaultPosition, p.timer.vaultTimer / p.vaultDuration);


	//if (abs(Vector3f::Distance(currentPos, p.endVaultPosition)) < 3.f)
	if (p.timer.vaultTimer >= p.vaultDuration)
	{
		currentPos = p.endVaultPosition;
		p.isVaulting = false;

		//aContext.animationDataComponent.isDone = true;
	}

	p.controller->setFootPosition({ currentPos.x, currentPos.y, currentPos.z });
	//p.yVelocity = 0;
	// Kan vi l?gga till leap glitch :3c
	//p.finalVelocity = { p.xVelocity.x, p.yVelocity, p.xVelocity.y };

	if (!p.isVaulting)
	{
		myStateMachine->SetState(ePlayerClassStates::Ground, aContext);
	}
}
