#pragma once

class State
{
public:
	State() = default;
	virtual ~State() = default;

	virtual void OnEnter() { __noop; };
	virtual void OnExit() { __noop; };
	virtual void Update(const float& dt) { dt; __noop; };
	virtual void Render() { __noop; };
};
