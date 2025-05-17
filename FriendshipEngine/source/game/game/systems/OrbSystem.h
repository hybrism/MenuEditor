#pragma once
#include <ecs\system\System.h>
#include <engine\math\Vector.h>
#include <iostream>
#include <deque>
#include <vector>
#include <../engine/engine/graphics/PostProcess.h>

class HitboxFactory;
struct TransformComponent;
struct PlayerComponent;
struct OrbComponent;
struct SceneUpdateContext;

class OrbSystem : public System<OrbSystem>
{
public:
	OrbSystem(World* aWorld);
	~OrbSystem() override;

	void Init() override;
	void Update(SceneUpdateContext& dt) override;
	void Render() override;

	void LoadInOrbData(std::string aFile,OrbComponent& aOrb);
 

private:
	void CreateFollowPath(PlayerComponent& aPlayer, const float aDT);
	void FollowPath(PlayerComponent& aPlayer,TransformComponent& atransform,OrbComponent& aOrb, const float aDT);
	void PlayerRegainHealth(PlayerComponent& aPlayer, OrbComponent& aOrb, const float& dt);
	void PlayerTakeDamage(PlayerComponent& aPlayer, OrbComponent& aOrb);
	void LerpVignette(const float& aDT, const float& aScaling);
	void SetVignetteRegular();
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

	//vignette
	PostProcess* myPostProcess;
	float myNewVignetteStrength;
	float myPreviousVigStrength;
	PostProcess::VignetteData myVigData;

};