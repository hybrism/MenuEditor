#pragma once

class ScriptableEventSystem : public System
{
public:
	ScriptableEventSystem(World* aWorld);

	// Inherited via System
	virtual void Update(const float& dt) override;

};

