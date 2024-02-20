#pragma once

class State;

class StateMachine
{
public:
	StateMachine();
	~StateMachine();

	void SetState(State* aState);
	void Update(const float& dt);
	void Render();

	State* GetCurrentState() const;
private:
	State* myCurrentState;
};