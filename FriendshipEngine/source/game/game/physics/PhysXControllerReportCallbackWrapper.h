#pragma once
#include <functional>
#include <PhysX/PxPhysicsAPI.h>
#include <PhysX/characterkinematic/PxController.h>

using namespace physx;

class PhysXControllerReportCallbackWrapper : public physx::PxUserControllerHitReport
{
public:
	void onShapeHit(const physx::PxControllerShapeHit& hit) override
	{
		OnShapeHit(hit);
	}

	void onControllerHit(const physx::PxControllersHit& hit) override
	{
		OnControllerHit(hit);
	}

	void onObstacleHit(const physx::PxControllerObstacleHit& hit) override
	{
		OnObstacleHit(hit);
	}

	std::function<void(const physx::PxControllerShapeHit&)>		OnShapeHit;
	std::function<void(const physx::PxControllersHit&)>			OnControllerHit;
	std::function<void(const physx::PxControllerObstacleHit&)>	OnObstacleHit;
};
