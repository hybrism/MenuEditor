#pragma once

class State
{
public:
	State();
	virtual ~State();

	virtual void OnEnter() {};
	virtual void OnExit() {};
	virtual void Update(const float& dt) { dt; };
	virtual void Render() {};
};
