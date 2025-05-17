#pragma once

class HitboxSystem : public System<HitboxSystem>
{
public:
	HitboxSystem(World* aWorld);
	//~HitboxSystem() override;





	// Inherited via System
	virtual void Update(SceneUpdateContext& dt) override;

};

