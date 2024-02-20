#pragma once

class HitboxSystem : public System
{
public:
	HitboxSystem(World* aWorld);
	//~HitboxSystem() override;





	// Inherited via System
	virtual void Update(const float& dt) override;

};

