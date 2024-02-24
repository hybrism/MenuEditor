#pragma once

class PlayerState
{
public:
	PlayerState() = default;
	virtual ~PlayerState() = default;

	virtual void OnEnter() = 0;
	virtual void OnExit() = 0;
	virtual void Update(const float& dt) = 0;
	virtual int GetID() { return myStateID; };

	int myStateID = -1;
};
