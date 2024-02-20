#pragma once

#include <ecs/entity/Entity.h>
#include <unordered_map>
#include <nlohmann/json.hpp>

class World;
class PhysXSceneManager;

class Scene;

using Json = nlohmann::json;

// We currently only support positive UnityIDs, since it will otherwise be out of bounds with the MAX_ENTITIES define
class UnityImporter
{
public:
	void LoadComponents(const std::string& aLevelName, World* aScene, PhysXSceneManager& aPhysXSceneManager);

private:
	
	void LoadEntities(const Json& aJson, World* aWorld);
	void LoadTransformComponent(const Json& aJson, World* aWorld);
	void LoadMetaDataComponent(const Json& aJson, World* aWorld);
	void LoadMeshComponent(const Json& aJson, World* aWorld);
	void LoadBoxColliderComponent(const Json& aJson, World* aWorld, PhysXSceneManager& aPhysXSceneManager);
	void LoadCameraComponent(const Json& aJson, World* aWorld);
	void LoadAnimationComponent(const Json& aJson, World* aWorld);
	void LoadPlayerComponents(const Json& aJson, World* aWorld, PhysXSceneManager& aPhysXSceneManager);
	void LoadEnemyComponent(const Json& aJson, World* aWorld);
	void LoadEventComponent(const Json& aJson, World* aWorld);
	void LoadDirectionalLightComponent(const Json& aJson, World* aWorld);
	void LoadPhysXComponent(const Json& aJson, World* aWorld, PhysXSceneManager& aPhysXSceneManager);
	void LoadOrbComponent(const Json& aJson, World* aWorld);
	void LoadProjectileComponent(const Json& aJson, World* aWorld);
	void LoadNpcComponent(const Json& aJson, World* aWorld);
	void InitializeAnimationControllers();

	Vector3f GetVector3f(const Json& aJson);
	std::unordered_map<int, eid_t> unityToEntity;
};
