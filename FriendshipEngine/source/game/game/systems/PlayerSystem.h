#pragma once

#include <engine/graphics/Camera.h>
#include <engine/math/AABB3D.h>

#include "playerStates/PlayerStatesInclude.h"
#include <physics/PhysXSceneManager.h>

struct PlayerComponent;
enum class ePlayerState;
class PhysXSceneManager;


#include "../playerStates/PlayerStateMachine.h"

class PlayerSystem : public System<PlayerSystem>
{
public:
	PlayerSystem(World* aWorld, PhysXSceneManager* aManager);
	~PlayerSystem() override;

	void Init() override;
	void Update(SceneUpdateContext& dt) override;
private:
	void Input(Entity entity, float dt);
	void CameraMove(Entity entity, float dt);
	void Collision(Entity entity, float dt);
	void VaultCollision(Entity entity, float dt);
	void Respawn(Entity entity);

	PhysXSceneManager* myPhysXSceneManager;
	float myMouseSensitivyMultiplier = 1.0f;

	PlayerStateMachine myStateMachine;

	PxControllerFilters filter;
};
