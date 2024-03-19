#pragma once
#include <PhysX/PxPhysicsAPI.h>

using namespace physx;

struct TransformComponent;
struct PhysXComponent;
struct ColliderComponent;
class PhysXSceneManager;

class PhysXSystem : public System<PhysXSystem>
{
public:
	// Inherited via System
	PhysXSystem(World* aWorld, PhysXSceneManager* aPhysXSceneManager);
	~PhysXSystem() override;

	void Init();
	void Update(const SceneUpdateContext& dt) override;

	//Quaternion conversion to matrix
	void QuatToMat(PhysXComponent& aPhysX, TransformComponent& aTransform);

private:
	PhysXSceneManager* myPhysXSceneManager;
};