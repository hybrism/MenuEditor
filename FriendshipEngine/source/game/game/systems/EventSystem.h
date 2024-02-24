#pragma once


class EventSystem : public System
{
public:
	EventSystem(World* aWorld);
	//~HitboxSystem() override;





	// Inherited via System
	virtual void Update(const float& dt) override;


private:
};

