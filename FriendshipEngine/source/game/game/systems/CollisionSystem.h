#pragma once

#include "../collision/Bitmask.h"
#include "../collision/CollisionLayers.h"

struct ColliderComponent;

class DebugLine;

class CollisionSystem : public System
{
public:
	CollisionSystem(World* aWorld);
	~CollisionSystem() override;

	void Update(const float&) override;
	void Render() override;

private:

	bool IsCloseEnoughToCheckCollision(Entity aFirstEntity, Entity aSecondEntity);
	bool IsColliding(Entity aFirstEntity, Entity aSecondEntity);
	bool LayerCheck(Entity aFirstEntity, Entity aSecondEntity);

	void DrawAllColliders();

	void SetupCollisionLayers();

	Ray<float> CreateRayFromMousePosition();

private:
	const float myDistanceToCheckCollision = 300.f;

	std::map<eCollisionLayer, Bitmask> myCollisionLayers;

	std::vector<DebugLine> myLines;
};