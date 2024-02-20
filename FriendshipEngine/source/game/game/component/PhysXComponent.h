#pragma once

namespace physx
{
 class PxRigidDynamic;
}

enum class PhysXType
{
	STATIC,
	DYNAMIC
};

struct PhysXComponent : public Component<PhysXComponent>
{
	Vector3f positionOffset = { 0, 0, 0 };
	physx::PxRigidDynamic* dynamicPhysX = nullptr;
};