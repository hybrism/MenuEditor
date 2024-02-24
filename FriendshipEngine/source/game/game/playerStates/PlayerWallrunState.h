#pragma once
#include "PlayerState.h"



class PlayerWallrunState : public PlayerState
{
public:
	void OnEnter() override;
	void OnExit() override;
	void Update(const float& dt) override;
	int GetID() override { return myStateID; };

	int myStateID = 4;
};