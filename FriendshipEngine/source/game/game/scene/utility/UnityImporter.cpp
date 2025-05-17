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
#include "../../component/NpcComponent.h"
#include "../../component/AnimationDataComponent.h"
#include "../../component/PhysXComponent.h"
#include "../../component/OrbComponent.h"
#include "../../component/ProjectileComponent.h"
#include "../../component/DeathZoneComponent.h"
#include "../../component/SphereColliderComponent.h"

#include "../../factory/ProjectileFactory.h"
#include "../../component/EventComponent.h"

#include <engine/graphics/GraphicsEngine.h>
#include <engine\graphics\Light\LightStructs.h>
#include <engine/graphics\animation\AnimationController.h>
#include <assets/AssetDatabase.h>
#include "physics/PhysXSceneManager.h"
#include "../../utility/JsonUtility.h"
#include "utility\GameHelperFunctions.h"

#ifndef _EDITOR
#include <assets/BinaryReader/BinaryVertexPaintingFileFactory.h>
#endif

static bool hasLoaded = false;

void UnityImporter::LoadComponents(
	const std::string& aLevelName,
	World* aWorld,
	PhysXSceneManager& aPhysXSceneManager,
	SceneUpdateContext& aContext
)
{
	std::string levelnameWithExtension = aLevelName;

	//Add extension if it does not exist
	size_t n = levelnameWithExtension.rfind(".json");
	if (n == std::string::npos)
	{
		levelnameWithExtension += ".json";
	}

	//Go to folder -> find .json
	std::string fullPath = RELATIVE_IMPORT_PATH + aLevelName + "/" + levelnameWithExtension;
	Json content = JsonUtility::OpenJson(fullPath);

	aWorld->Reset();

	LoadEntities(content, aWorld);
	LoadMetaDataComponent(content, aWorld);
	LoadTransformComponent(content, aWorld);
	LoadMeshComponent(aLevelName, content, aWorld);
	LoadPlayerComponents(content, aWorld, aPhysXSceneManager);
	LoadPhysXColliderComponent(content, aWorld, aPhysXSceneManager);
	LoadEventColliderComponent(content, aWorld);
	LoadEventComponent(content, aWorld);
	LoadCameraComponent(content, aWorld);
	LoadEnemyComponent(content, aWorld);
	LoadAnimationComponent(content, aWorld);
	LoadDirectionalLight(content, aWorld);
	LoadPointLight(content, aWorld);
	LoadOrbComponent(content, aWorld);
	LoadNpcComponent(content, aWorld);
	LoadDeathZoneComponent(content, aWorld);
	LoadSphereColliderComponent(content, aWorld);
	LoadParticleSystemComponent(content, aWorld, aContext);

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

	Skeleton* skeleton = nullptr;
	AnimationController* controller = nullptr;
	Animation* anim = nullptr;

	// Player
	{
		AnimationTransition defaultTransition;
		defaultTransition.duration = 0.1f;
		defaultTransition.exitTime = 0.0f;
		defaultTransition.isInterruptable = true;
		defaultTransition.transitionOffset = 0.0f;
		defaultTransition.fixedDuration = false;

		float minRunSpeed = 4.0f;
		float minFallSpeed = -10.5f;


		skeleton = AssetDatabase::GetSkeleton("sk_player");
		
		controller = &AssetDatabase::GetAnimationController(skeleton);

		controller->AddParameter("xSpeed", static_cast<size_t>(PlayerAnimationParameter::eXSpeed));
		controller->AddParameter("ySpeed", static_cast<size_t>(PlayerAnimationParameter::eYSpeed));
		controller->AddParameter("isSliding", static_cast<size_t>(PlayerAnimationParameter::eIsSliding));
		controller->AddParameter("wallrunDirection", static_cast<size_t>(PlayerAnimationParameter::eWallrunDirection)); // -1 left, 0 none, 1 right

		// Idle
		{
			anim = AssetDatabase::GetAnimation(skeleton, "a_player_idle");

			AnimationState state = CreateState(anim);

			{
				AnimationTransition transition = defaultTransition;
				transition.toStateIndex = static_cast<int>(PlayerAnimationState::eRun);
				transition.duration = 0.3f;

				transition.conditions.push_back(AnimationCondition(
					controller->GetParameterIndexFromName("xSpeed"),
					minRunSpeed,
					AnimationConditionType::eIsGreaterThan)
				);

				state.AddTransition(transition);
			}

			controller->AddState(state);
		}
		// Run
		{
			anim = AssetDatabase::GetAnimation(skeleton, "a_player_run");
			AnimationState state = CreateState(anim);

			//// Run to fall
			{
				AnimationTransition transition = defaultTransition;
				transition.duration = 0.1f;
				transition.toStateIndex = static_cast<int>(PlayerAnimationState::eJumpFall);

				transition.conditions.push_back(AnimationCondition(
					controller->GetParameterIndexFromName("ySpeed"),
					minFallSpeed,
					AnimationConditionType::eIsLessThan
				));

				state.AddTransition(transition);
			}

			// Run to idle
			{
				AnimationTransition transition = defaultTransition;
				transition.duration = 0.3f;
				transition.toStateIndex = static_cast<int>(PlayerAnimationState::eIdle);

				transition.conditions.push_back(AnimationCondition(
					controller->GetParameterIndexFromName("xSpeed"),
					minRunSpeed,
					AnimationConditionType::eIsLessThan)
				);

				state.AddTransition(transition);
			}

			// Run to slide
			{
				AnimationTransition transition = defaultTransition;
				transition.toStateIndex = static_cast<int>(PlayerAnimationState::eSlide);

				transition.conditions.push_back(AnimationCondition(
					controller->GetParameterIndexFromName("isSliding"),
					AnimationConditionType::eIsTrue)
				);

				state.AddTransition(transition);
			}

			// Run to wall run left
			{

				AnimationTransition transition = defaultTransition;
				transition.toStateIndex = static_cast<int>(PlayerAnimationState::eLeftWallRun);

				transition.conditions.push_back(AnimationCondition(
					controller->GetParameterIndexFromName("wallrunDirection"),
					-1,
					AnimationConditionType::eIsEqual
				));

				state.AddTransition(transition);
			}

			// Run to wall run right
			{

				AnimationTransition transition = defaultTransition;
				transition.toStateIndex = static_cast<int>(PlayerAnimationState::eRightWallRun);

				transition.conditions.push_back(AnimationCondition(
					controller->GetParameterIndexFromName("wallrunDirection"),
					1,
					AnimationConditionType::eIsEqual
				));

				state.AddTransition(transition);
			}

			controller->AddState(state);
		}

		// Slide
		{
			anim = AssetDatabase::GetAnimation(skeleton, "a_player_slide");
			AnimationState state = CreateState(anim);
			state.SetLooping(false);

			// Slide to fall
			{
				AnimationTransition transition = defaultTransition;
				transition.toStateIndex = static_cast<int>(PlayerAnimationState::eJumpFall);

				transition.conditions.push_back(AnimationCondition(
					controller->GetParameterIndexFromName("isSliding"),
					AnimationConditionType::eIsFalse)
				);

				transition.conditions.push_back(AnimationCondition(
					controller->GetParameterIndexFromName("ySpeed"),
					minFallSpeed,
					AnimationConditionType::eIsLessThan)
				);

				state.AddTransition(transition);
			}

			// Slide to run
			{
				AnimationTransition transition = defaultTransition;
				transition.fixedDuration = false;
				transition.duration = 0.15f;
				transition.toStateIndex = static_cast<int>(PlayerAnimationState::eRun);

				transition.conditions.push_back(AnimationCondition(
					controller->GetParameterIndexFromName("isSliding"),
					AnimationConditionType::eIsFalse)
				);

				transition.conditions.push_back(AnimationCondition(
					controller->GetParameterIndexFromName("xSpeed"),
					minRunSpeed,
					AnimationConditionType::eIsGreaterThan)
				);

				state.AddTransition(transition);
			}

			// Slide to idle
			{
				AnimationTransition transition = defaultTransition;
				transition.toStateIndex = static_cast<int>(PlayerAnimationState::eIdle);

				transition.conditions.push_back(AnimationCondition(
					controller->GetParameterIndexFromName("isSliding"),
					AnimationConditionType::eIsFalse)
				);

				transition.conditions.push_back(AnimationCondition(
					controller->GetParameterIndexFromName("xSpeed"),
					minRunSpeed,
					AnimationConditionType::eIsLessThan)
				);

				state.AddTransition(transition);
			}

			controller->AddState(state);
		}

		// Jump Fall
		{
			anim = AssetDatabase::GetAnimation(skeleton, "a_player_jumpFall");
			AnimationState state = CreateState(anim);
			state.SetLooping(false);

			// Fall to idle
			{
				AnimationTransition transition = defaultTransition;
				transition.duration = 0.15f;
				transition.fixedDuration = false;
				transition.toStateIndex = static_cast<int>(PlayerAnimationState::eIdle);

				transition.conditions.push_back(AnimationCondition(
					controller->GetParameterIndexFromName("ySpeed"),
					minFallSpeed,
					AnimationConditionType::eIsGreaterThan
				));

				transition.conditions.push_back(AnimationCondition(
					controller->GetParameterIndexFromName("xSpeed"),
					minRunSpeed,
					AnimationConditionType::eIsLessThan)
				);

				state.AddTransition(transition);
			}

			// Fall to run
			{
				AnimationTransition transition = defaultTransition;
				transition.duration = 0.15f;
				transition.fixedDuration = false;
				transition.toStateIndex = static_cast<int>(PlayerAnimationState::eRun);

				transition.conditions.push_back(AnimationCondition(
					controller->GetParameterIndexFromName("ySpeed"),
					minFallSpeed,
					AnimationConditionType::eIsGreaterThan
				));

				transition.conditions.push_back(AnimationCondition(
					controller->GetParameterIndexFromName("xSpeed"),
					minRunSpeed,
					AnimationConditionType::eIsGreaterThan)
				);

				state.AddTransition(transition);
			}

			// Fall to wall run left
			{
				AnimationTransition transition = defaultTransition;
				transition.duration = 0.15f;
				transition.fixedDuration = false;
				transition.toStateIndex = static_cast<int>(PlayerAnimationState::eLeftWallRun);

				transition.conditions.push_back(AnimationCondition(
					controller->GetParameterIndexFromName("wallrunDirection"),
					-1,
					AnimationConditionType::eIsEqual
				));

				state.AddTransition(transition);
			}

			// Fall to wall run right
			{
				AnimationTransition transition = defaultTransition;
				transition.duration = 0.15f;
				transition.fixedDuration = false;
				transition.toStateIndex = static_cast<int>(PlayerAnimationState::eRightWallRun);

				transition.conditions.push_back(AnimationCondition(
					controller->GetParameterIndexFromName("wallrunDirection"),
					1,
					AnimationConditionType::eIsEqual
				));

				state.AddTransition(transition);
			}

			controller->AddState(state);
		}

		// Left Wall Run
		{
			anim = AssetDatabase::GetAnimation(skeleton, "a_player_leftWallRun");
			AnimationState state = CreateState(anim);

			// Wall Run to Run
			{
				AnimationTransition transition = defaultTransition;
				transition.duration = 0.3f;
				transition.toStateIndex = static_cast<int>(PlayerAnimationState::eJumpFall);

				transition.conditions.push_back(AnimationCondition(
					controller->GetParameterIndexFromName("wallrunDirection"),
					0,
					AnimationConditionType::eIsEqual)
				);

				transition.conditions.push_back(AnimationCondition(
					controller->GetParameterIndexFromName("ySpeed"),
					minFallSpeed,
					AnimationConditionType::eIsGreaterThan)
				);

				state.AddTransition(transition);
			}

			// Wall Run To JumpFall
			{
				AnimationTransition transition = defaultTransition;
				transition.duration = 0.3f;
				transition.toStateIndex = static_cast<int>(PlayerAnimationState::eJumpFall);

				transition.conditions.push_back(AnimationCondition(
					controller->GetParameterIndexFromName("wallrunDirection"),
					0,
					AnimationConditionType::eIsEqual)
				);

				transition.conditions.push_back(AnimationCondition(
					controller->GetParameterIndexFromName("ySpeed"),
					minFallSpeed,
					AnimationConditionType::eIsLessThan)
				);

				state.AddTransition(transition);
			}

			controller->AddState(state);
		}

		// Right Wall Run
		{
			anim = AssetDatabase::GetAnimation(skeleton, "a_player_rightWallRun");
			AnimationState state = CreateState(anim);

			// Wall Run to Run
			{
				AnimationTransition transition = defaultTransition;
				transition.duration = 0.3f;
				transition.toStateIndex = static_cast<int>(PlayerAnimationState::eJumpFall);

				transition.conditions.push_back(AnimationCondition(
					controller->GetParameterIndexFromName("wallrunDirection"),
					0,
					AnimationConditionType::eIsEqual)
				);

				transition.conditions.push_back(AnimationCondition(
					controller->GetParameterIndexFromName("ySpeed"),
					minFallSpeed,
					AnimationConditionType::eIsGreaterThan)
				);

				state.AddTransition(transition);
			}

			// Wall Run To JumpFall
			{
				AnimationTransition transition = defaultTransition;
				transition.duration = 0.3f;
				transition.toStateIndex = static_cast<int>(PlayerAnimationState::eJumpFall);

				transition.conditions.push_back(AnimationCondition(
					controller->GetParameterIndexFromName("wallrunDirection"),
					0,
					AnimationConditionType::eIsEqual)
				);

				transition.conditions.push_back(AnimationCondition(
					controller->GetParameterIndexFromName("ySpeed"),
					minFallSpeed,
					AnimationConditionType::eIsLessThan)
				);

				state.AddTransition(transition);
			}

			controller->AddState(state);
		}

		// Vault
		{
			anim = AssetDatabase::GetAnimation(skeleton, "a_player_vault");
			AnimationState state = CreateState(anim);

			AnimationTransition defaultVaultTransition = defaultTransition;
			defaultVaultTransition.fixedDuration = true;
			defaultVaultTransition.duration = 0.1f;
			defaultVaultTransition.exitTime = 0.5f;

			// Vault to Run
			{
				AnimationTransition transition = defaultVaultTransition;
				transition.toStateIndex = static_cast<int>(PlayerAnimationState::eRun);

				transition.conditions.push_back(AnimationCondition(
					controller->GetParameterIndexFromName("xSpeed"),
					minRunSpeed,
					AnimationConditionType::eIsGreaterThan)
				);

				state.AddTransition(transition);
			}

			// Vault to Idle
			{
				AnimationTransition transition = defaultVaultTransition;
				transition.toStateIndex = static_cast<int>(PlayerAnimationState::eIdle);

				transition.conditions.push_back(AnimationCondition(
					controller->GetParameterIndexFromName("xSpeed"),
					minRunSpeed,
					AnimationConditionType::eIsLessThan)
				);

				state.AddTransition(transition);
			}

			controller->AddState(state);
		}
	}

	// Enemy
	{
		skeleton = AssetDatabase::GetSkeleton("sk_enemy");
		controller = &AssetDatabase::GetAnimationController(skeleton);

		controller->AddParameter("state", 0);

		// Recoil Down
		{
			anim = AssetDatabase::GetAnimation(skeleton, "a_enemy_recoilDown");
			AnimationState state = CreateState(anim);
			//	state.SetLooping(false);
			controller->AddState(state);
		}

		// Recoil Front
		{
			anim = AssetDatabase::GetAnimation(skeleton, "a_enemy_recoilFront");
			AnimationState state = CreateState(anim);

			controller->AddState(state);
		}
	}

	//NPC

	{
		skeleton = AssetDatabase::GetSkeleton("sk_npc");
		controller = &AssetDatabase::GetAnimationController(skeleton);

		controller->AddParameter("state", 0);

		// walk
		{
			anim = AssetDatabase::GetAnimation(skeleton, "a_npc_walk");
			AnimationState state = CreateState(anim);
			state.SetLooping(true);

			AnimationTransition transition;
			transition.duration = 0.1f;

			// n?r man s?ger att en animation skall bytas m?ste den passa detta v?rde i time procentuellt
			transition.exitTime = 0.0f; // exempelvis 0.2f inneb?r att animationen m?ste ha spelats i 20% av dess totala tid

			transition.toStateIndex = 1;

			// kopiera dessa
			transition.isInterruptable = true;
			transition.transitionOffset = 0.0f;
			transition.fixedDuration = false;

			transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("state"), 0, AnimationConditionType::eIsEqual));

			state.AddTransition(transition);

			controller->AddState(state);
		}

		// dead
		{
			anim = AssetDatabase::GetAnimation(skeleton, "a_npc_dead");
			AnimationState state = CreateState(anim);
			state.SetLooping(false);

			AnimationTransition transition;
			transition.duration = 0.1f;

			// n?r man s?ger att en animation skall bytas m?ste den passa detta v?rde i time procentuellt
			transition.exitTime = 0.0f; // exempelvis 0.2f inneb?r att animationen m?ste ha spelats i 20% av dess totala tid

			transition.toStateIndex = 0;

			// kopiera dessa
			transition.isInterruptable = true;
			transition.transitionOffset = 0.0f;
			transition.fixedDuration = false;

			transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("state"), 1, AnimationConditionType::eIsEqual));

			state.AddTransition(transition);
			controller->AddState(state);


		}

	}
}

bool UnityImporter::DoesContain(const Json& aJson, const std::string& aString)
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
#ifndef _RELEASE

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
#ifndef _RELEASE
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

#ifndef _RELEASE
	for (auto& n : aJson["names"])
	{
		int unityID = n["entityID"];
		Entity entity = unityToEntity.at(unityID);
		std::string name = n["name"];


		MetaDataComponent& metaData = aWorld->AddComponent<MetaDataComponent>(entity);
		std::strcpy(metaData.name, name.c_str());
		metaData.size = (unsigned int)std::min(name.size(), ARRAYSIZE(metaData.name));
		metaData.unityID = unityID;
	}
#endif
}

void UnityImporter::LoadMeshComponent(const std::string& aLevelName, const Json& aJson, World* aWorld)
{
#ifndef _RELEASE
	if (!DoesContain(aJson, "meshFilters"))
	{
		return;
	}
#endif

#ifdef _EDITOR
	myVertexPaintedIndexCounter = BinaryVertexPaintingFileFactory::LoadVertexIndicesFromFile(aLevelName);
	VertexIndexCollection& collection = myVertexPaintedIndexCounter;
#else
	VertexIndexCollection collection = BinaryVertexPaintingFileFactory::LoadVertexIndicesFromFile(aLevelName);
#endif
	for (auto& m : aJson["meshFilters"])
	{
		Entity entity = unityToEntity.at(m["entityID"]);
		size_t unityMeshID = m["meshID"];

		auto& mesh = aWorld->AddComponent<MeshComponent>(entity);
		mesh.id = AssetDatabase::GetMeshIdFromUnityId(unityMeshID);

		auto& package = AssetDatabase::GetMesh(mesh.id);
		mesh.type = package.skeleton ? MeshType::Skeletal : MeshType::Static;
		
		for (size_t i = 0; i < package.meshData.size(); i++)
		{
			if (AssetDatabase::GetTextureDatabase().TryGetVertexTextureIndex(package.name, i) >= 0)
			{
				if (collection.vertexIndices.find(entity) != collection.vertexIndices.end())
				{
					mesh.vertexPaintedIndex = collection.vertexIndices[entity];
					break;
				}
			}
		}
	}
}

void UnityImporter::LoadPhysXColliderComponent(const Json& aJson, World* aWorld, PhysXSceneManager& aPhysXSceneManager)
{
#ifndef _RELEASE
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

		Transform globalTransform = aWorld->GetComponent<TransformComponent>(entity).GetWorldTransform(aWorld, entity);
		aPhysXSceneManager.CreateStatic(globalTransform, extents);
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
		Vector3f pos = GetVector3f(event["pivot"]);
		Vector3f size;
		size.x = event["size"]["x"];
		size.y = event["size"]["y"];
		size.z = event["size"]["z"];
		size = size * 100.f;

		ColliderComponent& collider = aWorld->AddComponent<ColliderComponent>(id);

		collider.extents = size / 2.0f;
		collider.aabb3D.InitWithMinAndMax(Vector3f{ 0,0,0 }, size);

		CollisionDataComponent& collisionData = aWorld->AddComponent<CollisionDataComponent>(id);
		collisionData.type = eCollisionType::Trigger;
		collisionData.ownerID = id;
	}
}

void UnityImporter::LoadEventComponent(const Json& aJson, World* aWorld)
{
	if (!DoesContain(aJson, "eventComponents"))
	{
		return;
	}

	for (auto& event : aJson["eventComponents"])
	{
		Entity id = unityToEntity.at(event["entityID"]);

		auto& eventComp = aWorld->AddComponent<EventComponent>(id);
		eventComp.targetID = unityToEntity.at(event["targetID"]);
		eventComp.eventToTrigger = (eEvent)event["eventType"];
	}
}

void UnityImporter::LoadCameraComponent(const Json& aJson, World* aWorld)
{
#ifndef _RELEASE
	if (!DoesContain(aJson, "cameraComponent"))
	{
		return;
	}
#endif


	for (auto& cameraComponent : aJson["cameraComponent"])
	{
		Entity id = unityToEntity.at(cameraComponent["entityID"]);
		auto& camera = aWorld->AddComponent<CameraComponent>(id);
		camera.Pos.x = cameraComponent["pos"]["x"];
		camera.Pos.y = cameraComponent["pos"]["y"];
		camera.Pos.z = cameraComponent["pos"]["z"];

		camera.Pos *= 100.f;

		camera.Rot.x = cameraComponent["rot"]["x"];
		camera.Rot.y = cameraComponent["rot"]["y"];
		camera.Rot.z = cameraComponent["rot"]["z"];

		//camera.Rot *= 100.f;

	}
}

void UnityImporter::LoadPlayerComponents(const Json& aJson, World* aWorld, PhysXSceneManager& aPhysXSceneManager)
{
#ifndef _RELEASE
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
		playerComponent.SpawnPoint = transformComponent.transform.GetPosition();

		playerComponent.startGameRot.x = transformComponent.transform.GetEulerRotation().x;
		playerComponent.startGameRot.y = transformComponent.transform.GetEulerRotation().y;

		if (!aWorld->HasComponent<MeshComponent>(player))
		{
			auto& meshComponent = aWorld->AddComponent<MeshComponent>(player);
			meshComponent.id = AssetDatabase::GetMeshIdFromName("sk_player");
			meshComponent.shouldDisregardDepth = true;
			meshComponent.type = MeshType::Skeletal;

		}
		auto& meshComponent = aWorld->GetComponent<MeshComponent>(player);
		meshComponent.shouldDisregardDepth = true;

		if (!aWorld->HasComponent<AnimationDataComponent>(player))
		{
			auto& animComponent = aWorld->AddComponent<AnimationDataComponent>(player);
			Skeleton* skeleton = AssetDatabase::GetSkeleton(meshComponent.id);

			animComponent.controllerId = (int)AssetDatabase::GetAnimationController(skeleton).GetId();

			if (AssetDatabase::GetAnimations(skeleton).size() == 0) { continue; }

			animComponent.currentStateIndex = 0;
		}
	}
}
void UnityImporter::LoadAnimationComponent(const Json& aJson, World* aWorld)
{
#ifndef _RELEASE
	if (!DoesContain(aJson, "animationComponents"))
	{
		return;
	}
#endif

	for (auto& animationDataComponent : aJson["animationComponents"])
	{
		Entity id = unityToEntity.at(animationDataComponent["entityID"]);
		if (aWorld->HasComponent<AnimationDataComponent>(id)) { continue; }

		auto& mesh = aWorld->GetComponent<MeshComponent>(id);
		auto& component = aWorld->AddComponent<AnimationDataComponent>(id);
		Skeleton* skeleton = AssetDatabase::GetSkeleton(mesh.id);

		component.controllerId = AssetDatabase::GetAnimationController(skeleton).GetId();

		if (AssetDatabase::GetAnimations(skeleton).size() == 0) { continue; }

		component.currentStateIndex = 0;

#ifndef _RELEASE
		if (AssetDatabase::DoesAnimationControllerExist(skeleton)) { continue; }

		PrintW("We have animations but no controller!");

#endif // DEBUG
	}
}

void UnityImporter::LoadEnemyComponent(const Json& aJson, World* aWorld)
{
#ifndef _RELEASE
	if (!DoesContain(aJson, "enemyComponents"))
	{
		return;
	}
#endif


	for (auto& e : aJson["enemyComponents"])
	{
		Entity entity = unityToEntity.at(e["entityID"]);
		auto& transform = aWorld->GetComponent<TransformComponent>(entity);
		auto& enemyComp = aWorld->AddComponent<EnemyComponent>(entity);
		enemyComp.range = e["range"];
		enemyComp.attackSpeed = e["attackSpeed"];
		enemyComp.myPos = transform.transform.GetPosition();
		enemyComp.overlapShapePos.x = e["Position"]["x"];
		enemyComp.overlapShapePos.y = e["Position"]["y"];
		enemyComp.overlapShapePos.z = e["Position"]["z"];
		Vector3f size;
		size.x = e["Scale"]["x"];
		size.y = e["Scale"]["y"];
		size.z = e["Scale"]["z"];
		size = size * 100.f;
		Vector3f colliderSize = size / 2.f;
		enemyComp.sizeCollider = colliderSize;

		enemyComp.overlapShapePos.x *= 100;
		enemyComp.overlapShapePos.y *= 100;
		enemyComp.overlapShapePos.z *= 100;



	}
}

//void UnityImporter::LoadEventComponent(const Json& aJson, World* aWorld)
//{
//#ifndef _RELEASE
//	if (!DoesContain(aJson, "eventComponents"))
//	{
//		return;
//	}
//#endif
//
//	for (auto& event : aJson["eventComponents"])
//	{
//		Entity entity = unityToEntity.at(event["entityID"]);
//
//		//Set collider to trigger
//		auto& collisionData = aWorld->GetComponent<CollisionDataComponent>(entity);
//		collisionData.type = eCollisionType::Trigger;
//		collisionData.layer = eCollisionLayer::Event;
//
//		auto& scriptableEvent = aWorld->AddComponent<ScriptableEventComponent>(entity);
//		scriptableEvent.eventToTrigger = event["eventToTrigger"];
//		scriptableEvent.triggerEntityID = unityToEntity.at(event["targetEntityID"]);
//	}
//}


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
#ifndef _RELEASE
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

void UnityImporter::LoadDeathZoneComponent(const Json& aJson, World* aWorld)
{

#ifndef _RELEASE
	if (!DoesContain(aJson, "deathZoneComponent"))
	{
		return;
	}
#endif

	for (auto& d : aJson["deathZoneComponent"])
	{
		Entity entity = unityToEntity.at(d["entityID"]);
		auto& deathZone = aWorld->AddComponent<DeathZoneComponent>(entity);

		deathZone.overlapPos.x = d["myPos"]["x"];
		deathZone.overlapPos.y = d["myPos"]["y"];
		deathZone.overlapPos.z = d["myPos"]["z"];

		deathZone.overlapPos *= 100.f;

		Vector3f size;
		size.x = d["myScale"]["x"];
		size.y = d["myScale"]["y"];
		size.z = d["myScale"]["z"];
		size = size * 100.f;
		Vector3f colliderSize = size / 2.f;
		deathZone.myScale = colliderSize;

		/*	DirectX::XMMATRIX myMatrix = aWorld->GetComponent<TransformComponent>(entity).GetWorldTransform(aWorld, entity);
			Vector3f myPos = { myMatrix.r[3].m128_f32[0],myMatrix.r[3].m128_f32[1] ,myMatrix.r[3].m128_f32[2] };

			deathZone.StartPos = myPos;*/

	}

}

void UnityImporter::LoadNpcComponent(const Json& aJson, World* aWorld)
{
#ifndef _RELEASE
	if (!DoesContain(aJson, "nonPlayerComponent"))
	{
		return;
	}
#endif

	for (auto& npc : aJson["nonPlayerComponent"])
	{
		Entity entity = unityToEntity.at(npc["entityID"]);
		auto& npccomp = aWorld->AddComponent<NpcComponent>(entity);
		//auto& transformComponent = aWorld->GetComponent<TransformComponent>(entity);



		npccomp.ID = npc["myNPCID"];
		npccomp.GroupID = npc["myGroupID"];
		npccomp.DelayTimer = npc["myStartPathDelayTimer"];
		npccomp.movementSpeed = npc["myMovementSpeed"];
		npccomp.IsActive = npc["myIsActive"];
		npccomp.walkToPos.x = npc["PointPos"]["x"];
		npccomp.walkToPos.y = npc["PointPos"]["y"];
		npccomp.walkToPos.z = npc["PointPos"]["z"];

		npccomp.walkToPos.x *= 100;
		npccomp.walkToPos.y *= 100;
		npccomp.walkToPos.z *= 100;


		//transformComponent.transform.SetPosition(globalTransform);

		DirectX::XMMATRIX myMatrix = aWorld->GetComponent<TransformComponent>(entity).GetWorldTransform(aWorld, entity);
		Vector3f myPos = { myMatrix.r[3].m128_f32[0],myMatrix.r[3].m128_f32[1] ,myMatrix.r[3].m128_f32[2] };

		npccomp.StartPos = myPos;

		//	transformComponent.transform.SetPosition(npccomp.StartPos);


	}
}

void UnityImporter::LoadSphereColliderComponent(const Json& aJson, World* aWorld)
{
#ifndef _RELEASE
	if (!DoesContain(aJson, "sphereColliders"))
	{
		return;
	}
#endif
	for (auto& sphere : aJson["sphereColliders"])
	{
		Entity id = unityToEntity.at(sphere["entityID"]);
		auto& transform = aWorld->GetComponent<TransformComponent>(id).GetWorldTransform(aWorld, id);;

		auto& comp = aWorld->AddComponent<SphereColliderComponent>(id);
		comp.radius = (float)sphere["radius"] * 500.f;  //Increase or Decrease this amount depending on how big the spheres of the culled object should be.
		comp.center.x =(float)sphere["centerOffset"]["x"] * 100;
		comp.center.y =(float)sphere["centerOffset"]["y"] * 100;
		comp.center.z =(float)sphere["centerOffset"]["z"] * 100;
		comp.center.x += transform.r[3].m128_f32[0];
		comp.center.y += transform.r[3].m128_f32[1];
		comp.center.z += transform.r[3].m128_f32[2];
	}
}


#include <engine/graphics/vfx/ParticleSystemManager.h>
void UnityImporter::LoadParticleSystemComponent(const Json& aJson, World* aWorld, SceneUpdateContext& aContext)
{
	if (!DoesContain(aJson, "particleSystemComponents"))
	{
		return;
	}
	aWorld;
	for (auto& particle : aJson["particleSystemComponents"])
	{
		Entity entity = unityToEntity.at(particle["entityID"]);

		Transform& transform = aWorld->GetComponent<TransformComponent>(entity).transform;

		Vector3f offset = {
			(float)particle["offset"]["x"],
			(float)particle["offset"]["y"],
			(float)particle["offset"]["z"]
		};

		ParticleEmitter emitter;
		emitter.spawnAngle = 90.0f;
		emitter.spawnRadius = 150.0f;
		emitter.spawnRate = 100.0f;
		emitter.lifeSpan = 1.0f;
		emitter.speed = 100.0f;
		emitter.size = 1.0f;
		emitter.sizeVariation = 0.1f;

		emitter.position = transform.GetPosition() + offset;
		emitter.velocity = { 0, 10, 0 };
		emitter.color = Vector4f(255.0f / 255.0f, 221.0f / 255.0f, 99.0f / 255.0f, 1);

		size_t id = aContext.particleSystemManager->InsertEffect(emitter);
		aContext.particleSystemManager->Play(id);
	}
}

void UnityImporter::LoadDirectionalLight(const Json& aJson, World* aWorld)
{
	if (!DoesContain(aJson, "directionalLightComponentData"))
	{
		return;
	}

	for (auto& dirLight : aJson["directionalLightComponentData"])
	{
		Entity entity = unityToEntity.at(dirLight["entityID"]);

		Vector3f direction = aWorld->GetComponent<TransformComponent>(entity).transform.GetForward();

		Vector3f color = {
			color.x = (float)dirLight["color"]["x"],
			color.y = (float)dirLight["color"]["y"],
			color.z = (float)dirLight["color"]["z"]
		};

		float intensity = dirLight["intensity"];

		DirectionalLight dirrLight(direction, color, intensity);
		dirrLight.myTransform = aWorld->GetComponent<TransformComponent>(entity).transform;
		AssetDatabase::StoreDirectionalLight(dirrLight);
	}
}

void UnityImporter::LoadPointLight(const Json& aJson, World* aWorld)
{
	if (!DoesContain(aJson, "lightComponents"))
	{
		return;
	}
	AssetDatabase::ClearPointLight();
	for (auto& pointLight : aJson["lightComponents"])
	{
		Entity entity = unityToEntity.at(pointLight["entityID"]);

		Vector3f color = {
			color.x = (float)pointLight["color"]["x"] * 255.f,
			color.y = (float)pointLight["color"]["y"] * 255.f,
			color.z = (float)pointLight["color"]["z"] * 255.f
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