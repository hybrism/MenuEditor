#pragma once
#include "PlayerState.h"

class PlayerGroundState : public PlayerState
{
public:
	PlayerGroundState(PlayerStateMachine* aStateMachine);
	void OnEnter(PlayerStateUpdateContext& aContext) override;
	void OnExit(PlayerStateUpdateContext& aContext) override;
	void Update(PlayerStateUpdateContext& aContext) override;
private:
};