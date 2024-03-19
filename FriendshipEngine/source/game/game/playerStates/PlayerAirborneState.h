#pragma once
#include "PlayerState.h"

class PlayerAirborneState : public PlayerState
{
public:
	PlayerAirborneState(PlayerStateMachine* aStateMachine);
	void OnEnter(PlayerStateUpdateContext& aContext) override;
	void OnExit(PlayerStateUpdateContext& aContext) override;
	void Update(PlayerStateUpdateContext& aContext) override;
private:
	bool CanWallrun(PlayerStateUpdateContext& aContext);

	bool IsLookingAtWall(PlayerStateUpdateContext& aContext);
	bool IsLookingAtWallForMagnetEffect(PlayerStateUpdateContext& aContext);
};