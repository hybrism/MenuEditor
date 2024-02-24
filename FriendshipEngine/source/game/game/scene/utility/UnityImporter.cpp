#include "pch.h"
#include "UnityImporter.h"
#include "scene/Scene.h"

#include <filesystem>

#include <engine/Paths.h>

#include <ecs/World.h>
#include "../../component/TransformComponent.h"
#include "../../component/MetaDataComponent.h"
#include "../../component/MeshComponent.h"
#include "../../component/PlayerComponent.h"
#include "../../component/EnemyComponent.h"
#include "../../component/CameraComponent.h"
#include "../../component/ColliderComponent.h"
#include "../../component/CollisionDataComponent.h"
#include "../../component/ScriptableEventComponent.h"
#include "../../component/NpcComponent.h"
#include "../../component/AnimationDataComponent.h"
#include "../../component/PhysXComponent.h"
#include "../../component/OrbComponent.h"
#include "../../component/ProjectileComponent.h"
#include "../../factory/ProjectileFactory.h"

//#include <engine/graphics/DirectionalLightManager.h>
#include <engine/graphics/GraphicsEngine.h>
#include <engine\graphics\Light\LightStructs.h>
#include <engine/graphics\animation\AnimationController.h>
#include <assets/AssetDatabase.h>
#include "physics/PhysXSceneManager.h"
#include "../../utility/JsonUtility.h"

static bool hasLoaded = false;

void UnityImporter::LoadComponents(const std::string& aLevelName, World* aWorld, PhysXSceneManager& aPhysXSceneManager)
{
	std::string extension = aLevelName.substr(aLevelName.find_last_of(".") + 1);
	Json content;

	if (extension == "json")
	{
		content = JsonUtility::OpenJson(RELATIVE_IMPORT_PATH + aLevelName);
	}
	else if (extension == "friend")
	{
		std::ifstream t(RELATIVE_IMPORT_PATH + aLevelName, std::ios::binary);

		if (!t.is_open())
		{
			PrintE("[UnityImporter.cpp] Could not load friend-file! " + aLevelName);
			return;
		}

		content = Json::from_bson(t);
		t.close();
	}

	aWorld->Reset();

	LoadEntities(content, aWorld);
	LoadMetaDataComponent(content, aWorld);
	LoadTransformComponent(content, aWorld);
	LoadPlayerComponents(content, aWorld, aPhysXSceneManager);
	LoadMeshComponent(content, aWorld);
	LoadPhysXColliderComponent(content, aWorld, aPhysXSceneManager); // BoxCollider MUST be loaded before entities tries to GetComponent<CollisionData>
	LoadEventColliderComponent(content, aWorld);
	LoadCameraComponent(content, aWorld);
	LoadEnemyComponent(content, aWorld);
	LoadAnimationComponent(content, aWorld);
	//LoadPhysXComponent(content, aWorld, aPhysXSceneManager);
	LoadDirectionalLight(content, aWorld);
	LoadPointLight(content, aWorld);
	LoadOrbComponent(content, aWorld);
	LoadNpcComponent(content, aWorld);

	unityToEntity.clear();

	//TODO: Fix this! This was an emergency-fix so we don't load animationcontrollers multiple times
	if (!hasLoaded)
	{
		InitializeAnimationControllers();
		hasLoaded = true;
	}
}

AnimationState UnityImporter::CreateState(const Animation* aAnimation) const
{
	return AnimationState(static_cast<int>(AssetDatabase::GetAnimationIndex(aAnimation->name)));
}

// TODO: AnimationControllers are initialized here, but this should be revamped when we switch from unity
void UnityImporter::InitializeAnimationControllers()
{
	auto anims = AssetDatabase::GetAnimationMap();

	size_t meshId = 0;
	AnimationController* controller = nullptr;
	Animation* anim = nullptr;

	// Player
	{
		meshId = AssetDatabase::GetMeshIdFromName("sk_player");
		controller = AssetDatabase::GetAnimationController(meshId);

		controller->AddParameter("state", 0);

		// Idle
		{
			anim = AssetDatabase::GetAnimation(meshId, "a_player_idle");

			AnimationState state = CreateState(anim);

			AnimationTransition transition;
			transition.duration = 0.1f;

			// n?r man s?ger att en animation skall bytas m?ste den passa detta v?rde i time procentuellt
			transition.exitTime = 0.0f; // exempelvis 0.2f inneb?r att animationen m?ste ha spelats i 20% av dess totala tid

			transition.fromStateIndex = 0;
			transition.toStateIndex = 1;

			// kopiera dessa
			transition.isInterruptable = true;
			transition.transitionOffset = 0.0f;
			transition.fixedDuration = false;

			transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("state"), 0, AnimationConditionType::eIsEqual));

			state.AddTransition(transition);

			controller->AddState(state);
		}
		// Jump Fall
		{
			anim = AssetDatabase::GetAnimation(meshId, "a_player_jumpFall");
			AnimationState state = CreateState(anim);

			AnimationTransition transition;
			transition.duration = 0.1f;

			// n?r man s?ger att en animation skall bytas m?ste den passa detta v?rde i time procentuellt
			transition.exitTime = 0.0f; // exempelvis 0.2f inneb?r att animationen m?ste ha spelats i 20% av dess totala tid

			transition.fromStateIndex = 1;
			transition.toStateIndex = 2;

			// kopiera dessa
			transition.isInterruptable = true;
			transition.transitionOffset = 0.0f;
			transition.fixedDuration = false;

			transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("state"), 1, AnimationConditionType::eIsEqual));

			state.AddTransition(transition);

			controller->AddState(state);
		}
		// Left Wall Run
		{
			anim = AssetDatabase::GetAnimation(meshId, "a_player_leftWallRun");
			AnimationState state = CreateState(anim);
			AnimationTransition transition;
			transition.duration = 0.1f;

			// n?r man s?ger att en animation skall bytas m?ste den passa detta v?rde i time procentuellt
			transition.exitTime = 0.0f; // exempelvis 0.2f inneb?r att animationen m?ste ha spelats i 20% av dess totala tid

			transition.fromStateIndex = 2;
			transition.toStateIndex = 3;

			// kopiera dessa
			transition.isInterruptable = true;
			transition.transitionOffset = 0.0f;
			transition.fixedDuration = false;

			transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("state"), 2, AnimationConditionType::eIsEqual));

			state.AddTransition(transition);
			controller->AddState(state);
		}
		// Right Wall Run
		{
			anim = AssetDatabase::GetAnimation(meshId, "a_player_rightWallRun");
			AnimationState state = CreateState(anim);
			AnimationTransition transition;
			transition.duration = 0.1f;

			// n?r man s?ger att en animation skall bytas m?ste den passa detta v?rde i time procentuellt
			transition.exitTime = 0.0f; // exempelvis 0.2f inneb?r att animationen m?ste ha spelats i 20% av dess totala tid

			transition.fromStateIndex = 3;
			transition.toStateIndex = 4;

			// kopiera dessa
			transition.isInterruptable = true;
			transition.transitionOffset = 0.0f;
			transition.fixedDuration = false;

			transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("state"), 3, AnimationConditionType::eIsEqual));

			state.AddTransition(transition);
			controller->AddState(state);
		}
		// Run
		{
			anim = AssetDatabase::GetAnimation(meshId, "a_player_run");
			AnimationState state = CreateState(anim);
			AnimationTransition transition;
			transition.duration = 0.1f;

			// n?r man s?ger att en animation skall bytas m?ste den passa detta v?rde i time procentuellt
			transition.exitTime = 0.0f; // exempelvis 0.2f inneb?r att animationen m?ste ha spelats i 20% av dess totala tid

			transition.fromStateIndex = 4;
			transition.toStateIndex = 5;

			// kopiera dessa
			transition.isInterruptable = true;
			transition.transitionOffset = 0.0f;
			transition.fixedDuration = false;

			transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("state"), 4, AnimationConditionType::eIsEqual));

			state.AddTransition(transition);
			controller->AddState(state);
		}
		// Slide
		{
			anim = AssetDatabase::GetAnimation(meshId, "a_player_slide");
			AnimationState state = CreateState(anim);
			{

				AnimationTransition transition;
				transition.duration = 0.50f;

				// n?r man s?ger att en animation skall bytas m?ste den passa detta v?rde i time procentuellt
				transition.exitTime = 0.0f; // exempelvis 0.2f inneb?r att animationen m?ste ha spelats i 20% av dess totala tid

				transition.fromStateIndex = 5;
				transition.toStateIndex = 0;

				// kopiera dessa
				transition.isInterruptable = true;
				transition.transitionOffset = 0.0f;
				transition.fixedDuration = false;

				transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("state"), 5, AnimationConditionType::eIsEqual));
				state.AddTransition(transition);
			}

			controller->AddState(state);
		}
	}

	// Enemy
	{
		meshId = AssetDatabase::GetMeshIdFromName("sk_enemy");
		controller = AssetDatabase::GetAnimationController(meshId);

		controller->AddParameter("state", 0);

		// Recoil Down
		{
			anim = AssetDatabase::GetAnimation(meshId, "a_enemy_recoilDown");
			AnimationState state = CreateState(anim);

			controller->AddState(state);
		}

		// Recoil Front
		{
			anim = AssetDatabase::GetAnimation(meshId, "a_enemy_recoilFront");
			AnimationState state = CreateState(anim);

			controller->AddState(state);
		}
	}

	//NPC

	{
		//meshId = AssetDatabase::GetMeshIdFromName("sk_npc");
		//controller = AssetDatabase::GetAnimationController(meshId);

		//controller->AddParameter("state", 0);

		//// walk
		//{
		//	anim = AssetDatabase::GetAnimation(meshId, "a_npc_walk");
		//	AnimationState state = CreateState(anim);

		//	controller->AddState(state);
		//}

	
	}
}

bool UnityImporter::DoesContain(const Json& aJson ,const std::string& aString)
{
	if (aJson.contains(aString))
	{
		return true;
	}
	
	PrintW("Json does not contain: " + aString);
	
	return false;
}

void UnityImporter::LoadEntities(const Json& aJson, World* aWorld)
{
#ifdef _DEBUG

	if (!DoesContain(aJson, "entities"))
	{
		return;
	}
#endif

	for (auto& id : aJson["entities"])
	{
		eid_t entity = aWorld->CreateEntity();
		unityToEntity.insert({ id, entity });
	}
}

void UnityImporter::LoadTransformComponent(const Json& aJson, World* aWorld)
{
#ifdef _DEBUG
	if (!DoesContain(aJson, "transforms"))
	{
		return;
	}
#endif

	for (auto& t : aJson["transforms"])
	{
		Entity entity = unityToEntity.at(t["entityID"]);
		Vector3f position = GetVector3f(t["position"]);
		Vector3f eulerAngles = GetVector3f(t["rotation"]);
		Vector3f scale = GetVector3f(t["scale"]);

		// TODO: Only used for debugging, maybe could be used for something else?
		auto& transform = aWorld->AddComponent<TransformComponent>(entity);
		transform.transform.SetPosition(position * 100.f);
		transform.transform.SetEulerAngles(eulerAngles);

		if (t["parent"] != INVALID_ENTITY)
		{
			Entity parentID = unityToEntity.at(t["parent"]);
			transform.parent = parentID;
		}

		auto& children = t["children"];
		assert(children.size() <= MAX_CHILDREN && "Transform contains more children than currently allowed");
		for (size_t i = 0; i < children.size(); i++)
		{
			Entity childID = unityToEntity.at(children[i]);
			transform.children[i] = childID;
		}
	}
}

void UnityImporter::LoadMetaDataComponent(const Json& aJson, World* aWorld)
{
	aJson;
	aWorld;

#ifdef _DEBUG
	for (auto& n : aJson["names"])
	{
		int unityID = n["entityID"];
		Entity entity = unityToEntity.at(unityID);
		std::string name = n["name"];

		MetaDataComponent& metaData = aWorld->AddComponent<MetaDataComponent>(entity);
		std::strcpy(metaData.name, name.c_str());
		metaData.unityID = unityID;
	}
#endif
}

void UnityImporter::LoadMeshComponent(const Json& aJson, World* aWorld)
{
#ifdef _DEBUG
	if (!DoesContain(aJson, "meshFilters"))
	{
		return;
	}
#endif

	for (auto& m : aJson["meshFilters"])
	{
		Entity entity = unityToEntity.at(m["entityID"]);
		size_t meshID = m["meshID"];

		auto& mesh = aWorld->AddComponent<MeshComponent>(entity);
		mesh.id = AssetDatabase::GetMeshIdFromUnityId(meshID);

		if (aWorld->GetComponent<TransformComponent>(entity).parent == aWorld->GetPlayerEntityID())
		{
			mesh.shouldDisregardDepth = true;
		}

		auto& package = AssetDatabase::GetMesh(mesh.id);
		mesh.type = package.skeleton ? MeshType::Skeletal : MeshType::Static;
	}
}

void UnityImporter::LoadPhysXColliderComponent(const Json& aJson, World* aWorld, PhysXSceneManager& aPhysXSceneManager)
{
#ifdef _DEBUG
	if (!DoesContain(aJson, "boxColliders"))
	{
		return;
	}
#endif

	for (auto& box : aJson["boxColliders"])
	{
		Entity entity = unityToEntity.at(box["entityID"]);
		//auto& collider = aWorld->AddComponent<ColliderComponent>(entity);
		//auto& collisionData = aWorld->AddComponent<CollisionDataComponent>(entity);

		//collisionData.ownerID = entity;


		Vector3f size;
		size.x = box["size"]["x"];
		size.y = box["size"]["y"];
		size.z = box["size"]["z"];
		size = size * 100.f;
		Vector3f extents = size / 2.0f;
		//Vector3f center;
		//center.x = box["center"]["x"];
		//center.y = box["center"]["y"];
		//center.z = box["center"]["z"];
		//center = center * 100.0f;

		if (box["isDynamic"])
		{
			auto& physXComponent = aWorld->AddComponent<PhysXComponent>(entity);
			physXComponent.dynamicPhysX = aPhysXSceneManager.CreateDynamicBox(aWorld->GetComponent<TransformComponent>(entity).transform, extents);
			continue;
		}

		//Every collider that loads is default Collider type, if u want trigger Add EventComponent in Unity
		//collisionData.type = eCollisionType::Collider;


		aPhysXSceneManager.CreateStatic(aWorld->GetComponent<TransformComponent>(entity).transform, extents);
	}
}

void UnityImporter::LoadEventColliderComponent(const Json& aJson, World* aWorld)
{
	if (!DoesContain(aJson, "eventColliderComponents"))
	{
		return;
	}

	for (auto& event : aJson["eventColliderComponents"])
	{
		Entity id = unityToEntity.at(event["entityID"]);
		Vector3f pivot = GetVector3f(event["pivot"]);
		Vector3f size = GetVector3f(event["size"]);
		size *= 100.f;

		ColliderComponent& collider = aWorld->AddComponent<ColliderComponent>(id);
		collider.aabb3D.InitWithMinAndMax(pivot, pivot + size);
		collider.extents = size / 2.f;
		
		CollisionDataComponent& collisionData = aWorld->AddComponent<CollisionDataComponent>(id);
		collisionData.type = eCollisionType::Trigger;
		collisionData.ownerID = id;
	}
}

void UnityImporter::LoadCameraComponent(const Json& aJson, World* aWorld)
{
#ifdef _DEBUG
	if (!DoesContain(aJson, "cameraComponent"))
	{
		return;
	}
#endif


	for (auto& cameraComponent : aJson["cameraComponent"])
	{
		Entity id = unityToEntity.at(cameraComponent["entityID"]);
		aWorld->AddComponent<CameraComponent>(id);
	}
}

void UnityImporter::LoadPlayerComponents(const Json& aJson, World* aWorld, PhysXSceneManager& aPhysXSceneManager)
{
#ifdef _DEBUG
	if (!DoesContain(aJson, "playerComponent"))
	{
		aWorld->SetPlayerEntityID(INVALID_ENTITY);
		return;
	}
#endif

	Entity player;
	for (auto& entityID : aJson["playerComponent"])
	{
		player = unityToEntity.at(entityID);
		aWorld->SetPlayerEntityID(player);
		auto& playerComponent = aWorld->AddComponent<PlayerComponent>(player);
		auto& transformComponent = aWorld->GetComponent<TransformComponent>(player);
		playerComponent.controller = aPhysXSceneManager.CreateCharacterController(transformComponent.transform, &playerComponent.callbackWrapper);
	}
}
void UnityImporter::LoadAnimationComponent(const Json& aJson, World* aWorld)
{
#ifdef _DEBUG
	if (!DoesContain(aJson, "animationComponents"))
	{
		return;
	}
#endif

	for (auto& animationDataComponent : aJson["animationComponents"])
	{
		Entity id = unityToEntity.at(animationDataComponent["entityID"]);
		auto& mesh = aWorld->GetComponent<MeshComponent>(id);
		auto& component = aWorld->AddComponent<AnimationDataComponent>(id);

		if (AssetDatabase::GetAnimations(mesh.id).size() == 0) { continue; }

		component.currentStateIndex = 0;

#ifdef _DEBUG
		if (AssetDatabase::DoesAnimationControllerExist(mesh.id)) { continue; }

		PrintW("We have animations but no controller!");

#endif // DEBUG
	}
}

void UnityImporter::LoadEnemyComponent(const Json& aJson, World* aWorld)
{
#ifdef _DEBUG
	if (!DoesContain(aJson, "enemyComponents"))
	{
		return;
	}
#endif


	for (auto& e : aJson["enemyComponents"])
	{
		Entity entity = unityToEntity.at(e["entityID"]);

		auto& enemyComp = aWorld->AddComponent<EnemyComponent>(entity);
		enemyComp.attackSpeed = e["attackSpeed"];
	}
}

void UnityImporter::LoadEventComponent(const Json& aJson, World* aWorld)
{
#ifdef _DEBUG
	if (!DoesContain(aJson, "eventComponents"))
	{
		return;
	}
#endif

	for (auto& event : aJson["eventComponents"])
	{
		Entity entity = unityToEntity.at(event["entityID"]);

		//Set collider to trigger
		auto& collisionData = aWorld->GetComponent<CollisionDataComponent>(entity);
		collisionData.type = eCollisionType::Trigger;
		collisionData.layer = eCollisionLayer::Event;

		auto& scriptableEvent = aWorld->AddComponent<ScriptableEventComponent>(entity);
		scriptableEvent.eventToTrigger = event["eventToTrigger"];
		scriptableEvent.triggerEntityID = unityToEntity.at(event["targetEntityID"]);
	}
}


//void UnityImporter::LoadPhysXComponent(const Json& aJson, World* aWorld, PhysXSceneManager& aPhysXSceneManager)
//{
// 
// 	if (!DoesContain(aJson, "physXComponents"))
//{
//	return;
//}
//	using namespace physx;
//	for (auto& physX : aJson["physXComponents"])
//	{
//		Entity entity = unityToEntity.at(physX["entityID"]);
//		auto& physXComponent = aWorld->AddComponent<PhysXComponent>(entity);
//		auto& collider = aWorld->GetComponent<ColliderComponent>(entity);
//		//physXComponent.dynamicPhysX = cc->getActor();
//		collider;
//		physXComponent.dynamicPhysX = aPhysXSceneManager.CreateDynamicBox(aWorld->GetComponent<TransformComponent>(entity).transform, collider.extents);
//	}
//}

void UnityImporter::LoadOrbComponent(const Json& aJson, World* aWorld)
{
#ifdef _DEBUG
	if (!DoesContain(aJson, "orbComponent"))
	{
		return;
	}
#endif

	for (auto& orb : aJson["orbComponent"])
	{
		Entity id = unityToEntity.at(orb["entityID"]);
		aWorld->AddComponent<OrbComponent>(id);
	}
}

void UnityImporter::LoadNpcComponent(const Json& aJson, World* aWorld)
{
#ifdef _DEBUG
	if (!DoesContain(aJson, "nonPlayerComponent"))
	{
		return;
	}
#endif

	for (auto& npc : aJson["nonPlayerComponent"])
	{
		Entity entity = unityToEntity.at(npc["entityID"]);
		auto& npccomp = aWorld->AddComponent<NpcComponent>(entity);

		npccomp.ID = npc["myNPCID"];
		npccomp.GroupID = npc["myGroupID"];
		npccomp.DelayTimer = npc["myStartPathDelayTimer"];
		npccomp.IsActive = npc["myIsActive"];
		npccomp.walkToPos.x = npc["PointPos"]["x"];
		npccomp.walkToPos.y = npc["PointPos"]["y"];
		npccomp.walkToPos.z = npc["PointPos"]["z"];


		npccomp.walkToPos.x *= 100;
		npccomp.walkToPos.y *= 100;
		npccomp.walkToPos.z *= 100;

	}
}

void UnityImporter::LoadDirectionalLight(const Json& aJson, World* aWorld)
{
	if (!DoesContain(aJson, "directionalLightComponentData"))
	{
		return;
	}
	aWorld;
	for (auto& dirLight : aJson["directionalLightComponentData"])
	{
		Entity entity = unityToEntity.at(dirLight["entityID"]);

		Vector3f direction = aWorld->GetComponent<TransformComponent>(entity).transform.GetForward();


		Vector3f color = {
			color.x = dirLight["color"]["x"],
			color.y = dirLight["color"]["y"],
			color.z = dirLight["color"]["z"]
		};


		DirectionalLight dirrLight(direction, color, 1.f);
		AssetDatabase::StoreDirectionalLight(dirrLight);
	}
}

void UnityImporter::LoadPointLight(const Json& aJson, World* aWorld)
{
	if (!DoesContain(aJson, "lightComponents"))
	{
		return;
	}

	for (auto& pointLight : aJson["lightComponents"])
	{
		Entity entity = unityToEntity.at(pointLight["entityID"]);

		Vector3f color = {
			color.x = pointLight["color"]["x"] * 255.f,
			color.y = pointLight["color"]["y"] * 255.f,
			color.z = pointLight["color"]["z"] * 255.f
		};
		float intensity;
		intensity = pointLight["intensity"] * 100.f;
		float range;
		range = (float)pointLight["range"] * 100.f;
		Vector3f pos;
		pos = aWorld->GetComponent<TransformComponent>(entity).transform.GetPosition();
		PointLight light(pos, color, range, intensity);

		AssetDatabase::StorePointLight(light);
	}
}
Vector3f UnityImporter::GetVector3f(const Json& aJson)
{
	float x = aJson["x"].get<float>();
	float y = aJson["y"].get<float>();
	float z = aJson["z"].get<float>();
	return Vector3f{ x, y, z };
}