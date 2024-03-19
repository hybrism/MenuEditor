#pragma once
#include "PlayerState.h"

class PlayerCrouchState : public PlayerState
{
public:
	PlayerCrouchState(PlayerStateMachine* aStateMachine);
	void OnEnter(PlayerStateUpdateContext& aContext) override;
	void OnExit(PlayerStateUpdateContext& aContext) override;
	void Update(PlayerStateUpdateContext& aContext) override;
private:
	bool CanUncrouch(PlayerStateUpdateContext& aContext);
};