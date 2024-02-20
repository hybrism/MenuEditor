#pragma once

#include <engine/graphics/Camera.h>
#include <engine/math/AABB3D.h>

struct PlayerComponent;
enum class ePlayerState;
class PhysXSceneManager;

#include <engine/graphics/vfx/VFXManager.h>
class PlayerSystem : public System
{
public:
	PlayerSystem(World* aWorld, PhysXSceneManager* aManager);
	~PlayerSystem() override;

	void Init() override;
	void Update(const float& dt) override;
	void Render() override;
private:
	void Input(Entity entity, float dt);
	void Move(Entity entity, float dt);
	void CameraMove(Entity entity, float dt);
	void Collision(Entity entity, float dt);
	void VaultCollision(Entity entity, float dt);
	void Slide(Entity entity, float dt);

	VFXManager manager;
	vfxid_t testEffectId;
	void TestVfxBlob(const float& dt);

	PhysXSceneManager* myPhysXSceneManager;
};
