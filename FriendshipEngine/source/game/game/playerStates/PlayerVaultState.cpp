#include "pch.h"
#include "PlayerVaultState.h"
#include "PlayerStateMachine.h"
#include "../component/PlayerComponent.h"
#include <physics/PhysXSceneManager.h>

PlayerVaultState::PlayerVaultState(PlayerStateMachine* aStateMachine) : PlayerState(aStateMachine)
{
}

void PlayerVaultState::OnEnter(PlayerStateUpdateContext& aContext)
{
	PlayerComponent& p = aContext.playerComponent;
	p.currentCameraHeight = PlayerConstants::cameraHeight;
	p.controller->resize(CHARACTER_HEIGHT);
}

void PlayerVaultState::OnExit(PlayerStateUpdateContext&)
{
}

void PlayerVaultState::Update(PlayerStateUpdateContext& aContext)
{
	PlayerComponent& p = aContext.playerComponent;

	p.controller->setFootPosition({ p.vaultLocation.x, p.vaultLocation.y, p.vaultLocation.z });
	p.yVelocity = 0;

	// Kan vi lägga till leap glitch :3c
	p.finalVelocity = { p.xVelocity.x, p.yVelocity, p.xVelocity.y };

	myStateMachine->SetState(ePlayerClassStates::Ground, aContext);
}
