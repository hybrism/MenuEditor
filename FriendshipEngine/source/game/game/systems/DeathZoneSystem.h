#pragma once
#include "../component/DeathZoneComponent.h"

#include <physics/PhysXSceneManager.h>

struct PlayerComponent;

class DeathZoneSystem : public System<DeathZoneSystem>
{
public:
	DeathZoneSystem(World* aWorld, PhysXSceneManager* aPhysXSceneManager);
	~DeathZoneSystem() override;

	void Init() override;
	void Update(const SceneUpdateContext& dt) override;


private:

	bool IsInsideDeathZone(DeathZoneComponent& aDeath,PlayerComponent& aPlayer);
	bool myIsTrueOrFalse;
	PhysXSceneManager* myPhysXSceneManager;

};