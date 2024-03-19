#pragma once
#include "PlayerState.h"

class PlayerVaultState : public PlayerState
{
public:
	PlayerVaultState(PlayerStateMachine* aStateMachine);
	void OnEnter(PlayerStateUpdateContext& aContext) override;
	void OnExit(PlayerStateUpdateContext& aContext) override;
	void Update(PlayerStateUpdateContext& aContext) override;
private:
};