#pragma once
#include <ecs\system\System.h>
#include <engine\math\Vector.h>
#include <iostream>
#include <deque>
#include <vector>

class HitboxFactory;
struct TransformComponent;
struct PlayerComponent;
struct OrbComponent;

class OrbSystem : public System

{
public:
	OrbSystem(World* aWorld);
	~OrbSystem() override;

	void Init() override;
	void Update(const float& dt) override;
	void Render() override;

	void LoadInOrbData(std::string aFile,OrbComponent& aOrb);
 
	void CreateFollowPath(PlayerComponent& aPlayer, const float aDT);
	void FollowPath(PlayerComponent& aPlayer,TransformComponent& atransform,OrbComponent& aOrb, const float aDT);
	void PlayerRegainHealth(OrbComponent& aOrb, const float& dt);
	void PlayerTakeDamage(OrbComponent& aOrb);

private:
	float myCreationTimer = 0.f;
	float myChaseTimer = 0.f;
	float myChaseSpeedThreshold = 0.5f;

	float myMinSpeed = 400.f;

	int myPathIndex = 0;
	int mylength = 3;

	std::string ORB_DATA = "orbData.json";
	std::vector<Vector3f> myFollowPath;
	Vector3f myPlayerPos;
	std::vector<DebugLine> myFollowLines;
};