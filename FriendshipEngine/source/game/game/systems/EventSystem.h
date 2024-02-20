#pragma once

class StateStack;

class EventSystem : public System
{
public:
	EventSystem(World* aWorld);
	//~HitboxSystem() override;





	// Inherited via System
	virtual void Update(const float& dt) override;

	void Connect(StateStack* aStateStack);

private:
	StateStack* myStateStackPtr;
};

