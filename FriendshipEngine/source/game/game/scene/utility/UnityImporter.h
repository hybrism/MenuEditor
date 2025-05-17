#pragma once

#include <ecs/entity/Entity.h>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <engine/graphics/Animation/AnimationState.h>

class World;
class PhysXSceneManager;

class Scene;
struct Animation;
struct SceneUpdateContext;

using Json = nlohmann::json;

#ifdef _EDITOR
#include <assets/BinaryReader/BinaryVertexPaintingFileFactory.h>
#endif

// We currently only support positive UnityIDs, since it will otherwise be out of bounds with the MAX_ENTITIES define
class UnityImporter
{
public:
	void LoadComponents(
		const std::string& aLevelName,
		World* aScene,
		PhysXSceneManager& aPhysXSceneManager,
		SceneUpdateContext& aContext
	);

#ifdef _EDITOR
	VertexIndexCollection& GetVertexPaintedIndexCollection() { return myVertexPaintedIndexCounter; }
#endif
private:
	AnimationState CreateState(const Animation* aAnimation) const;
	void LoadEntities(const Json& aJson, World* aWorld);
	void LoadTransformComponent(const Json& aJson, World* aWorld);
	void LoadMetaDataComponent(const Json& aJson, World* aWorld);
	void LoadMeshComponent(const std::string& aLevelName, const Json& aJson, World* aWorld);
	void LoadPhysXColliderComponent(const Json& aJson, World* aWorld, PhysXSceneManager& aPhysXSceneManager);
	void LoadEventColliderComponent(const Json& aJson, World* aWorld);
	void LoadEventComponent(const Json& aJson, World* aWorld);
	void LoadCameraComponent(const Json& aJson, World* aWorld);
	void LoadAnimationComponent(const Json& aJson, World* aWorld);
	void LoadPlayerComponents(const Json& aJson, World* aWorld, PhysXSceneManager& aPhysXSceneManager);
	void LoadEnemyComponent(const Json& aJson, World* aWorld);
	void LoadOrbComponent(const Json& aJson, World* aWorld);
	void LoadDeathZoneComponent(const Json& aJson, World* aWorld);
	void LoadSphereColliderComponent(const Json& aJson, World* aWorld);
	void LoadParticleSystemComponent(const Json& aJson, World* aWorld, SceneUpdateContext& aContext);

	void LoadNpcComponent(const Json& aJson, World* aWorld);
	void LoadDirectionalLight(const Json& aJson, World* aWorld);
	void LoadPointLight(const Json& aJson, World* aWorld);
	void InitializeAnimationControllers();

	bool DoesContain(const Json& aJson,const std::string& aString);

	//TODO: Is this Ok to remove
	//void LoadPhysXComponent(const Json& aJson, World* aWorld, PhysXSceneManager& aPhysXSceneManager);
	
	Vector3f GetVector3f(const Json& aJson);
	std::unordered_map<int, eid_t> unityToEntity;
#ifdef _EDITOR
	VertexIndexCollection myVertexPaintedIndexCounter;
#endif
};
