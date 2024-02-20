#pragma once

class ScriptableEventSystem : public System
{
public:
	ScriptableEventSystem(World* aWorld);
	//~HitboxSystem() override;





	// Inherited via System
	virtual void Update(const float& dt) override;

};

