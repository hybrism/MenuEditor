#include "pch.h"
#include "PlayerStateMachine.h"

PlayerStateMachine::PlayerStateMachine() : myCurrentState(ePlayerClassStates::Count), myStates()
{
	for (int i = 0; i < static_cast<int>(ePlayerClassStates::Count); i++)
	{
		myStates[i] = nullptr;
	}
}

PlayerStateMachine::~PlayerStateMachine()
{
	for (int i = 0; i < static_cast<int>(ePlayerClassStates::Count); i++)
	{
		if (myStates[i] != nullptr)
		{
			delete myStates[i];
			myStates[i] = nullptr;
		}
	}
}

void PlayerStateMachine::Init()
{
	myStates[static_cast<int>(ePlayerClassStates::Ground)] = new PlayerGroundState(this);
	myStates[static_cast<int>(ePlayerClassStates::Airborne)] = new PlayerAirborneState(this);
	myStates[static_cast<int>(ePlayerClassStates::Crouch)] = new PlayerCrouchState(this);
	myStates[static_cast<int>(ePlayerClassStates::Slide)] = new PlayerSlideState(this);
	myStates[static_cast<int>(ePlayerClassStates::Wallrun)] = new PlayerWallrunState(this);
	myStates[static_cast<int>(ePlayerClassStates::Vault)] = new PlayerVaultState(this);
	
	myCurrentState = ePlayerClassStates::Ground;
}

void PlayerStateMachine::SetState(ePlayerClassStates aState, PlayerStateUpdateContext& aContext)
{
	if (myCurrentState == aState) { return; }

	if (myCurrentState != ePlayerClassStates::Count)
	{
		GetState()->OnExit(aContext);
	}
	myCurrentState = aState;
	GetState()->OnEnter(aContext);
}

void PlayerStateMachine::Update(PlayerStateUpdateContext& aContext)
{
	if (myCurrentState == ePlayerClassStates::Count) { return; }

	GetState()->Update(aContext);
}

PlayerState* PlayerStateMachine::GetState() const
{
	return myStates[static_cast<int>(myCurrentState)];
}
