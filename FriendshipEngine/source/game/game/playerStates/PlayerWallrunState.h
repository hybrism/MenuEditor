#pragma once
#include "PlayerState.h"

class PlayerWallrunState : public PlayerState
{
public:
	PlayerWallrunState(PlayerStateMachine* aStateMachine);
	void OnEnter(PlayerStateUpdateContext& aContext) override;
	void OnExit(PlayerStateUpdateContext& aContext) override;
	void Update(PlayerStateUpdateContext& aContext) override;
private:
	bool CanWallrun(PlayerStateUpdateContext& aContext);

	bool IsOnWallNextFrame(PlayerStateUpdateContext& aContext);

};