#pragma once
#include "PlayerState.h"

class PlayerSlideState : public PlayerState
{
public:
	PlayerSlideState(PlayerStateMachine* aStateMachine);
	void OnEnter(PlayerStateUpdateContext& aContext) override;
	void OnExit(PlayerStateUpdateContext& aContext) override;
	void Update(PlayerStateUpdateContext& aContext) override;
private:
	bool CanUncrouch(PlayerStateUpdateContext& aContext);
};