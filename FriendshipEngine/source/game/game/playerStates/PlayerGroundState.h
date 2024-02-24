#pragma once
#include "PlayerState.h"



class PlayerGroundState : public PlayerState
{
public:
	void OnEnter() override;
	void OnExit() override;
	void Update(const float& dt) override;
	int GetID() override { return 4; };
};