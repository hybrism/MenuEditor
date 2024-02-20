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
	void GenerateDebugLines(Entity aEntity);
	void UpdateDebugLines(Entity aEntity);
	void DrawSpecificCollider(Entity aEntity);

	void SetupCollisionLayers();

	void CheckHoveredEnemies();
	Ray<float> CreateRayFromMousePosition(Vector2<float> aMousePosition);
private:
	const float myDistanceToCheckCollision = 300.f;

	bool lineMade = false;
	float checkHoverTimer = 0;
	DebugLine* ray = nullptr;
	std::map<eCollisionLayer, Bitmask> myCollisionLayers;
};