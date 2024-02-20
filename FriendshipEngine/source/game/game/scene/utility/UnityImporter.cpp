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
	LoadMeshComponent(content, aWorld);
	LoadBoxColliderComponent(content, aWorld, aPhysXSceneManager); // BoxCollider MUST be loaded before entities tries to GetComponent<CollisionData>

	LoadCameraComponent(content, aWorld);
	LoadEnemyComponent(content, aWorld);
	LoadPlayerComponents(content, aWorld, aPhysXSceneManager);
	LoadAnimationComponent(content, aWorld);
	//LoadEventComponent(content, aWorld); // Event component MUST be loaded after BoxColliderComponent
	LoadPhysXComponent(content, aWorld, aPhysXSceneManager);
	LoadDirectionalLightComponent(content, aWorld);
	LoadOrbComponent(content, aWorld);
	LoadProjectileComponent(content, aWorld);
	LoadNpcComponent(content, aWorld);
	unityToEntity.clear();

	//TODO: Fix this! This was an emergency-fix so we don't load animationcontrollers multiple times
	if (!hasLoaded)
	{
		InitializeAnimationControllers();
		hasLoaded = true;
	}
}

// TODO: AnimationControllers are initialized here, but this should be revamped when we switch from unity
void UnityImporter::InitializeAnimationControllers()
{
	auto anims = AssetDatabase::GetAnimationMap();

	for (auto& pair : anims)
	{
		for (int i = 0; i < static_cast<int>(pair.second.size()); i++)
		{
			if (!AssetDatabase::DoesAnimationControllerExist(pair.first)) { continue; }

			//auto animationName = db->GetAnimationName(pair.first);
			auto* animation = pair.second[i];
			auto name = animation->name;
			auto animName = name;
			{
				auto index = name.find('_') + 1;
				name = name.substr(index, name.length() - index);

				index = name.find('_') + 1;
				animName = name.substr(index, name.length() - index);
				name = name.substr(0, index - 1);
			}

			auto* controller = AssetDatabase::GetAnimationController(pair.first);
			if (name == "enemy")
			{
				if (animName == "recoil")
				{

					controller->AddParameter("state", 0);

					AnimationState state = AnimationState(i);
					AnimationTransition transition = AnimationTransition();
					transition.duration = 0.1f;
					transition.exitTime = 0.0f;
					transition.fromStateIndex = 0;
					transition.toStateIndex = 0;
					transition.isInterruptable = true;
					transition.transitionOffset = 0.0f;
					transition.fixedDuration = false;


					transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("state"), 0, AnimationConditionType::eIsEqual));

					state.AddTransition(transition);
					controller->AddState(state);

					//AnimationState state = AnimationState(i);

					////MOVING
					//{
					//	AnimationTransition transition = AnimationTransition();
					//	// hur l?ng tid det tar att g? fr?n en animations state till en annan
					//	transition.duration = 0.1f;

					//	// n?r man s?ger att en animation skall bytas m?ste den passa detta v?rde i time procentuellt
					//	transition.exitTime = 0.0f; // exempelvis 0.2f inneb?r att animationen m?ste ha spelats i 20% av dess totala tid

					//	transition.fromStateIndex = 6;
					//	transition.toStateIndex = 8;

					//	// kopiera dessa
					//	transition.isInterruptable = true;
					//	transition.transitionOffset = 0.0f;
					//	transition.fixedDuration = false;

					//	transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("state"), 8, AnimationConditionType::eIsEqual));

					//	state.AddTransition(transition);
					//	controller->SetDefaultState(6);

				}
			}
			else if (name == "playerCharacter")
			{


				//AOE ABILITY
				if (animName == "ability01")
				{
					controller->AddParameter("state", 0);

					AnimationState state = AnimationState(i);
					AnimationTransition transition = AnimationTransition();
					transition.duration = 0.1f;
					transition.exitTime = 0.0f;
					transition.fromStateIndex = 0;
					transition.toStateIndex = 6;
					transition.isInterruptable = true;
					transition.transitionOffset = 0.0f;
					transition.fixedDuration = false;


					transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("state"), 6, AnimationConditionType::eIsEqual));

					state.AddTransition(transition);
					controller->AddState(state);
				}

				//dash ability
				else if (animName == "ability02")
				{
					AnimationState state = AnimationState(i);
					AnimationTransition transition = AnimationTransition();
					transition.duration = 0.1f;
					transition.exitTime = 0.0f;
					transition.fromStateIndex = 1;
					transition.toStateIndex = 6;
					transition.isInterruptable = true;
					transition.transitionOffset = 0.0f;
					transition.fixedDuration = false;


					transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("state"), 6, AnimationConditionType::eIsEqual));

					state.AddTransition(transition);
					controller->AddState(state);
				}

				//SHIELD ABLITY
				else if (animName == "ability03")
				{
					AnimationState state = AnimationState(i);
					AnimationTransition transition = AnimationTransition();
					transition.duration = 0.1f;
					transition.exitTime = 0.0f;
					transition.fromStateIndex = 2;
					transition.toStateIndex = 6;
					transition.isInterruptable = true;
					transition.transitionOffset = 0.0f;
					transition.fixedDuration = false;


					transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("state"), 6, AnimationConditionType::eIsEqual));

					state.AddTransition(transition);
					controller->AddState(state);
				}

				//ULTING ABILITY
				else if (animName == "ability04")
				{
					AnimationState state = AnimationState(i);
					AnimationTransition transition = AnimationTransition();
					transition.duration = 0.1f;
					transition.exitTime = 0.0f;
					transition.fromStateIndex = 3;
					transition.toStateIndex = 6;
					transition.isInterruptable = true;
					transition.transitionOffset = 0.0f;
					transition.fixedDuration = false;


					transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("state"), 6, AnimationConditionType::eIsEqual));

					state.AddTransition(transition);
					controller->AddState(state);
				}
				else if (animName == "death")
				{
					AnimationState state = AnimationState(i);
					AnimationTransition transition = AnimationTransition();
					transition.duration = 0.1f;
					transition.exitTime = 0.0f;
					transition.fromStateIndex = 4;
					transition.toStateIndex = 6;
					transition.isInterruptable = true;
					transition.transitionOffset = 0.0f;
					transition.fixedDuration = false;


					transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("state"), 6, AnimationConditionType::eIsEqual));

					state.AddTransition(transition);
					controller->AddState(state);
				}

				//HEAVY ATTACK ABILITY
				else if (animName == "heavyAttack")
				{
					AnimationState state = AnimationState(i);
					AnimationTransition transition = AnimationTransition();
					transition.duration = 0.1f;
					transition.exitTime = 0.0f;
					transition.fromStateIndex = 5;
					transition.toStateIndex = 6;
					transition.isInterruptable = true;
					transition.transitionOffset = 0.0f;
					transition.fixedDuration = false;


					transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("state"), 6, AnimationConditionType::eIsEqual));

					state.AddTransition(transition);
					controller->AddState(state);

				}

				else if (animName == "idleAnim")
				{

					AnimationState state = AnimationState(i);

					//MOVING
					{
						AnimationTransition transition = AnimationTransition();
						// hur l?ng tid det tar att g? fr?n en animations state till en annan
						transition.duration = 0.1f;

						// n?r man s?ger att en animation skall bytas m?ste den passa detta v?rde i time procentuellt
						transition.exitTime = 0.0f; // exempelvis 0.2f inneb?r att animationen m?ste ha spelats i 20% av dess totala tid

						transition.fromStateIndex = 6;
						transition.toStateIndex = 8;

						// kopiera dessa
						transition.isInterruptable = true;
						transition.transitionOffset = 0.0f;
						transition.fixedDuration = false;

						transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("state"), 8, AnimationConditionType::eIsEqual));

						state.AddTransition(transition);
						controller->SetDefaultState(6);

					}

					//To LMB
					{
						AnimationTransition transition = AnimationTransition();

						// hur l?ng tid det tar att g? fr?n en animations state till en annan
						transition.duration = 0.1f;

						// n?r man s?ger att en animation skall bytas m?ste den passa detta v?rde i time procentuellt
						transition.exitTime = 0.0f; // exempelvis 0.2f inneb?r att animationen m?ste ha spelats i 20% av dess totala tid

						transition.fromStateIndex = 6;
						transition.toStateIndex = 7;

						// kopiera dessa
						transition.isInterruptable = true;
						transition.transitionOffset = 0.0f;
						transition.fixedDuration = false;

						transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("state"), 7, AnimationConditionType::eIsEqual));
						state.AddTransition(transition);

					}
					//To AOE ABILITY
					{
						AnimationTransition transition = AnimationTransition();

						// hur l?ng tid det tar att g? fr?n en animations state till en annan
						transition.duration = 0.1f;

						// n?r man s?ger att en animation skall bytas m?ste den passa detta v?rde i time procentuellt
						transition.exitTime = 0.0f; // exempelvis 0.2f inneb?r att animationen m?ste ha spelats i 20% av dess totala tid

						transition.fromStateIndex = 6;
						transition.toStateIndex = 0;

						// kopiera dessa
						transition.isInterruptable = true;
						transition.transitionOffset = 0.0f;
						transition.fixedDuration = false;

						transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("state"), 0, AnimationConditionType::eIsEqual));
						state.AddTransition(transition);

					}

					//To DASH ABILITY
					{
						AnimationTransition transition = AnimationTransition();

						// hur l?ng tid det tar att g? fr?n en animations state till en annan
						transition.duration = 0.1f;

						// n?r man s?ger att en animation skall bytas m?ste den passa detta v?rde i time procentuellt
						transition.exitTime = 0.0f; // exempelvis 0.2f inneb?r att animationen m?ste ha spelats i 20% av dess totala tid

						transition.fromStateIndex = 6;
						transition.toStateIndex = 1;

						// kopiera dessa
						transition.isInterruptable = true;
						transition.transitionOffset = 0.0f;
						transition.fixedDuration = false;

						transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("state"), 1, AnimationConditionType::eIsEqual));
						state.AddTransition(transition);

					}

					//To SHIELD ABILITY
					{
						AnimationTransition transition = AnimationTransition();

						// hur l?ng tid det tar att g? fr?n en animations state till en annan
						transition.duration = 0.1f;

						// n?r man s?ger att en animation skall bytas m?ste den passa detta v?rde i time procentuellt
						transition.exitTime = 0.0f; // exempelvis 0.2f inneb?r att animationen m?ste ha spelats i 20% av dess totala tid

						transition.fromStateIndex = 6;
						transition.toStateIndex = 2;

						// kopiera dessa
						transition.isInterruptable = true;
						transition.transitionOffset = 0.0f;
						transition.fixedDuration = false;

						transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("state"), 2, AnimationConditionType::eIsEqual));
						state.AddTransition(transition);

					}
					//To ULTING ABILITY
					{
						AnimationTransition transition = AnimationTransition();

						// hur l?ng tid det tar att g? fr?n en animations state till en annan
						transition.duration = 0.1f;

						// n?r man s?ger att en animation skall bytas m?ste den passa detta v?rde i time procentuellt
						transition.exitTime = 0.0f; // exempelvis 0.2f inneb?r att animationen m?ste ha spelats i 20% av dess totala tid

						transition.fromStateIndex = 6;
						transition.toStateIndex = 3;

						// kopiera dessa
						transition.isInterruptable = true;
						transition.transitionOffset = 0.0f;
						transition.fixedDuration = false;

						transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("state"), 3, AnimationConditionType::eIsEqual));
						state.AddTransition(transition);

					}
					//To DEATH ANIMATION
					{
						AnimationTransition transition = AnimationTransition();

						// hur l?ng tid det tar att g? fr?n en animations state till en annan
						transition.duration = 0.1f;

						// n?r man s?ger att en animation skall bytas m?ste den passa detta v?rde i time procentuellt
						transition.exitTime = 0.0f; // exempelvis 0.2f inneb?r att animationen m?ste ha spelats i 20% av dess totala tid

						transition.fromStateIndex = 6;
						transition.toStateIndex = 4;

						// kopiera dessa
						transition.isInterruptable = true;
						transition.transitionOffset = 0.0f;
						transition.fixedDuration = false;

						transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("state"), 4, AnimationConditionType::eIsEqual));
						state.AddTransition(transition);

					}
					//To HAEVY ATTACK ANIMATION
					{
						AnimationTransition transition = AnimationTransition();

						// hur l?ng tid det tar att g? fr?n en animations state till en annan
						transition.duration = 0.1f;

						// n?r man s?ger att en animation skall bytas m?ste den passa detta v?rde i time procentuellt
						transition.exitTime = 0.0f; // exempelvis 0.2f inneb?r att animationen m?ste ha spelats i 20% av dess totala tid

						transition.fromStateIndex = 6;
						transition.toStateIndex = 5;

						// kopiera dessa
						transition.isInterruptable = true;
						transition.transitionOffset = 0.0f;
						transition.fixedDuration = false;

						transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("state"), 5, AnimationConditionType::eIsEqual));
						state.AddTransition(transition);

					}

					controller->AddState(state);
				}




				else if (animName == "lightAttack")
				{
					AnimationState state = AnimationState(i);
					AnimationTransition transition = AnimationTransition();
					transition.duration = 0.1f;
					transition.exitTime = 1.0f;
					transition.fromStateIndex = 7;
					transition.toStateIndex = 6;
					transition.isInterruptable = true;
					transition.transitionOffset = 0.0f;
					transition.fixedDuration = false;


					transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("state"), 6, AnimationConditionType::eIsEqual));

					state.AddTransition(transition);
					controller->AddState(state);

				}

				else if (animName == "runAnim")
				{
					AnimationState state = AnimationState(i);
					AnimationTransition transition = AnimationTransition();
					transition.duration = 0.1f;
					transition.exitTime = 0.0f;
					transition.fromStateIndex = 8;
					transition.toStateIndex = 6;
					transition.isInterruptable = true;
					transition.transitionOffset = 0.0f;
					transition.fixedDuration = false;
					transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("state"), 6, AnimationConditionType::eIsEqual));


					state.AddTransition(transition);

					controller->AddState(state);
				}


			}
			else if (name == "swampMonsterElite")
			{

			}
			else if (name == "swampMonster")
			{
				if (animName == "attackAnim")
				{
					controller->AddParameter("eliteEnemyStates", 0);

					AnimationState state = AnimationState(i);
					controller->AddState(state);
					AnimationTransition transition = AnimationTransition();

					transition.duration = 0.1f;
					transition.exitTime = 0.0f;
					transition.fromStateIndex = 0;
					transition.toStateIndex = 2;
					transition.isInterruptable = true;
					transition.transitionOffset = 0.0f;
					transition.fixedDuration = false;

					transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("eliteEnemyStates"), 2, AnimationConditionType::eIsEqual));

					state.AddTransition(transition);
					controller->AddState(state);
				}
				else if (animName == "deathAnim")
				{
					AnimationState state = AnimationState(i);
					controller->AddState(state);
					AnimationTransition transition = AnimationTransition();

					transition.duration = 0.1f;
					transition.exitTime = 0.0f;
					transition.fromStateIndex = 1;
					transition.toStateIndex = 2;
					transition.isInterruptable = true;
					transition.transitionOffset = 0.0f;
					transition.fixedDuration = false;
					transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("eliteEnemyStates"), 2, AnimationConditionType::eIsEqual));

					state.AddTransition(transition);
					controller->AddState(state);
				}
				else if (animName == "idleAnim")
				{

					AnimationState state = AnimationState(i);
					controller->AddState(state);
					AnimationTransition transition = AnimationTransition();

					transition.duration = 0.1f;
					transition.exitTime = 0.0f;
					transition.fromStateIndex = 2;
					transition.toStateIndex = 3;
					transition.isInterruptable = true;
					transition.transitionOffset = 0.0f;
					transition.fixedDuration = false;
					transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("eliteEnemyStates"), 3, AnimationConditionType::eIsEqual));

					state.AddTransition(transition);
					controller->AddState(state);
					controller->SetDefaultState(2);

				}
				else if (animName == "runAnim")
				{

					AnimationState state = AnimationState(i);
					controller->AddState(state);
					AnimationTransition transition = AnimationTransition();

					transition.duration = 0.1f;
					transition.exitTime = 0.0f;
					transition.fromStateIndex = 3;
					transition.toStateIndex = 0;
					transition.isInterruptable = true;
					transition.transitionOffset = 0.0f;
					transition.fixedDuration = false;
					transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("eliteEnemyStates"), 0, AnimationConditionType::eIsEqual));

					state.AddTransition(transition);
					controller->AddState(state);
				}
			}
			else if (name == "shaman")
			{


				if (animName == "death")
				{
					controller->AddParameter("shamanStates", 0);
					AnimationState state = AnimationState(i);
					AnimationTransition transition = AnimationTransition();
					transition.duration = 0.1f;
					transition.exitTime = 0.0f;
					transition.fromStateIndex = 0;
					transition.toStateIndex = 2;
					transition.isInterruptable = true;
					transition.transitionOffset = 0.0f;
					transition.fixedDuration = false;


					transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("shamanStates"), 2, AnimationConditionType::eIsEqual));

					state.AddTransition(transition);
					controller->AddState(state);
				}
				else if (animName == "heal")
				{
					AnimationState state = AnimationState(i);
					AnimationTransition transition = AnimationTransition();
					transition.duration = 0.1f;
					transition.exitTime = 0.0f;
					transition.fromStateIndex = 1;
					transition.toStateIndex = 2;
					transition.isInterruptable = true;
					transition.transitionOffset = 0.0f;
					transition.fixedDuration = false;


					transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("shamanStates"), 2, AnimationConditionType::eIsEqual));

					state.AddTransition(transition);
					controller->AddState(state);
				}
				else if (animName == "idle")
				{


					AnimationState state = AnimationState(i);

					//RUNNING
					{
						AnimationTransition transition = AnimationTransition();
						// hur l?ng tid det tar att g? fr?n en animations state till en annan
						transition.duration = 0.1f;

						// n?r man s?ger att en animation skall bytas m?ste den passa detta v?rde i time procentuellt
						transition.exitTime = 0.0f; // exempelvis 0.2f inneb?r att animationen m?ste ha spelats i 20% av dess totala tid

						transition.fromStateIndex = 2;
						transition.toStateIndex = 3;

						// kopiera dessa
						transition.isInterruptable = true;
						transition.transitionOffset = 0.0f;
						transition.fixedDuration = false;

						transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("shamanStates"), 3, AnimationConditionType::eIsEqual));

						state.AddTransition(transition);
						controller->SetDefaultState(2);

					}

					//To HEALING ABILITY
					{
						AnimationTransition transition = AnimationTransition();

						// hur l?ng tid det tar att g? fr?n en animations state till en annan
						transition.duration = 0.1f;

						// n?r man s?ger att en animation skall bytas m?ste den passa detta v?rde i time procentuellt
						transition.exitTime = 0.0f; // exempelvis 0.2f inneb?r att animationen m?ste ha spelats i 20% av dess totala tid

						transition.fromStateIndex = 2;
						transition.toStateIndex = 1;

						// kopiera dessa
						transition.isInterruptable = true;
						transition.transitionOffset = 0.0f;
						transition.fixedDuration = false;

						transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("shamanStates"), 1, AnimationConditionType::eIsEqual));
						state.AddTransition(transition);

					}

					//To DEATH 
					{
						AnimationTransition transition = AnimationTransition();

						// hur l?ng tid det tar att g? fr?n en animations state till en annan
						transition.duration = 0.1f;

						// n?r man s?ger att en animation skall bytas m?ste den passa detta v?rde i time procentuellt
						transition.exitTime = 0.0f; // exempelvis 0.2f inneb?r att animationen m?ste ha spelats i 20% av dess totala tid

						transition.fromStateIndex = 2;
						transition.toStateIndex = 0;

						// kopiera dessa
						transition.isInterruptable = true;
						transition.transitionOffset = 0.0f;
						transition.fixedDuration = false;

						transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("shamanStates"), 0, AnimationConditionType::eIsEqual));
						state.AddTransition(transition);

					}
					controller->AddState(state);
					controller->SetDefaultState(3);


				}
				else if (animName == "run")
				{
					AnimationState state = AnimationState(i);
					AnimationTransition transition = AnimationTransition();
					transition.duration = 0.1f;
					transition.exitTime = 0.0f;
					transition.fromStateIndex = 3;
					transition.toStateIndex = 2;
					transition.isInterruptable = true;
					transition.transitionOffset = 0.0f;
					transition.fixedDuration = false;


					transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("shamanStates"), 2, AnimationConditionType::eIsEqual));

					state.AddTransition(transition);
					controller->AddState(state);
				}


			}
			else if (name == "boss")
			{
				if (animName == "armswipe")
				{
					controller->AddParameter("AttackType", 0);

					AnimationState state = AnimationState(i);
					AnimationTransition transition = AnimationTransition();

					// hur l?ng tid det tar att g? fr?n en animations state till en annan
					transition.duration = 0.1f;

					// n?r man s?ger att en animation skall bytas m?ste den passa detta v?rde i time procentuellt
					transition.exitTime = 0.0f; // exempelvis 0.2f inneb?r att animationen m?ste ha spelats i 20% av dess totala tid

					transition.fromStateIndex = 0;
					transition.toStateIndex = 3;

					// kopiera dessa
					transition.isInterruptable = true;
					transition.transitionOffset = 0.0f;
					transition.fixedDuration = false;

					transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("AttackType"), 3, AnimationConditionType::eIsEqual));
					state.AddTransition(transition);
					controller->AddState(state);
				}
				else if (animName == "channel")
				{
					AnimationState state = AnimationState(i);
					AnimationTransition transition = AnimationTransition();

					// hur l?ng tid det tar att g? fr?n en animations state till en annan
					transition.duration = 0.1f;

					// n?r man s?ger att en animation skall bytas m?ste den passa detta v?rde i time procentuellt
					transition.exitTime = 1.0f; // exempelvis 0.2f inneb?r att animationen m?ste ha spelats i 20% av dess totala tid

					transition.fromStateIndex = 1;
					transition.toStateIndex = 3;

					// kopiera dessa
					transition.isInterruptable = true;
					transition.transitionOffset = 0.0f;
					transition.fixedDuration = false;

					transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("AttackType"), 3, AnimationConditionType::eIsEqual));
					state.AddTransition(transition);
					controller->AddState(state);
				}
				else if (animName == "fistslam")
				{
					AnimationState state = AnimationState(i);
					AnimationTransition transition = AnimationTransition();

					// hur l?ng tid det tar att g? fr?n en animations state till en annan
					transition.duration = 0.1f;

					// n?r man s?ger att en animation skall bytas m?ste den passa detta v?rde i time procentuellt
					transition.exitTime = 0.0f; // exempelvis 0.2f inneb?r att animationen m?ste ha spelats i 20% av dess totala tid

					transition.fromStateIndex = 2;
					transition.toStateIndex = 3;

					// kopiera dessa
					transition.isInterruptable = true;
					transition.transitionOffset = 0.0f;
					transition.fixedDuration = false;

					transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("AttackType"), 3, AnimationConditionType::eIsEqual));
					state.AddTransition(transition);
					controller->AddState(state);
				}
				else if (animName == "idle")
				{

					AnimationState state = AnimationState(i);

					//To fistslam
					{
						AnimationTransition transition = AnimationTransition();

						// hur l?ng tid det tar att g? fr?n en animations state till en annan
						transition.duration = 0.1f;

						// n?r man s?ger att en animation skall bytas m?ste den passa detta v?rde i time procentuellt
						transition.exitTime = 1.0f; // exempelvis 0.2f inneb?r att animationen m?ste ha spelats i 20% av dess totala tid

						transition.fromStateIndex = 3;
						transition.toStateIndex = 2;

						// kopiera dessa
						transition.isInterruptable = true;
						transition.transitionOffset = 0.0f;
						transition.fixedDuration = false;

						transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("AttackType"), 2, AnimationConditionType::eIsEqual));
						state.AddTransition(transition);
					}
					//To armswipe
					{
						AnimationTransition transition = AnimationTransition();

						// hur l?ng tid det tar att g? fr?n en animations state till en annan
						transition.duration = 0.1f;

						// n?r man s?ger att en animation skall bytas m?ste den passa detta v?rde i time procentuellt
						transition.exitTime = 0.0f; // exempelvis 0.2f inneb?r att animationen m?ste ha spelats i 20% av dess totala tid

						transition.fromStateIndex = 3;
						transition.toStateIndex = 0;

						// kopiera dessa
						transition.isInterruptable = true;
						transition.transitionOffset = 0.0f;
						transition.fixedDuration = false;

						transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("AttackType"), 0, AnimationConditionType::eIsEqual));
						state.AddTransition(transition);
					}
					//To channel
					{
						AnimationTransition transition = AnimationTransition();

						// hur l?ng tid det tar att g? fr?n en animations state till en annan
						transition.duration = 0.1f;

						// n?r man s?ger att en animation skall bytas m?ste den passa detta v?rde i time procentuellt
						transition.exitTime = 0.0f; // exempelvis 0.2f inneb?r att animationen m?ste ha spelats i 20% av dess totala tid

						transition.fromStateIndex = 3;
						transition.toStateIndex = 1;

						// kopiera dessa
						transition.isInterruptable = true;
						transition.transitionOffset = 0.0f;
						transition.fixedDuration = false;

						transition.conditions.push_back(AnimationCondition(controller->GetParameterIndexFromName("AttackType"), 1, AnimationConditionType::eIsEqual));
						state.AddTransition(transition);
					}

					controller->AddState(state);
					controller->SetDefaultState(3);
				}
			}
			// TODO: Initialize animation controller states here
		}
	}
}

void UnityImporter::LoadEntities(const Json& aJson, World* aWorld)
{
	for (auto& id : aJson["entities"])
	{
		eid_t entity = aWorld->CreateEntity();
		unityToEntity.insert({ id, entity });
	}
}

void UnityImporter::LoadTransformComponent(const Json& aJson, World* aWorld)
{
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

		// TODO: Only used for debugging, maybe could be used for something else?
		MetaDataComponent& metaData = aWorld->AddComponent<MetaDataComponent>(entity);
		std::strcpy(metaData.name, name.c_str());
		metaData.unityID = unityID;
	}
#endif
}

void UnityImporter::LoadMeshComponent(const Json& aJson, World* aWorld)
{
	for (auto& m : aJson["meshFilters"])
	{
		Entity entity = unityToEntity.at(m["entityID"]);
		size_t meshID = m["meshID"];

		auto& mesh = aWorld->AddComponent<MeshComponent>(entity);
		mesh.id = AssetDatabase::GetMeshIdFromUnityId(meshID);

		auto& package = AssetDatabase::GetMesh(mesh.id);
		mesh.type = package.skeleton ? MeshType::Skeletal : MeshType::Static;
	}
}

void UnityImporter::LoadBoxColliderComponent(const Json& aJson, World* aWorld, PhysXSceneManager& aPhysXSceneManager)
{
	for (auto& box : aJson["boxColliders"])
	{
		Entity entity = unityToEntity.at(box["entityID"]);
		auto& collider = aWorld->AddComponent<ColliderComponent>(entity);
		auto& collisionData = aWorld->AddComponent<CollisionDataComponent>(entity);

		collisionData.ownerID = entity;
		Vector3f size;
		size.x = box["size"]["x"];
		size.y = box["size"]["y"];
		size.z = box["size"]["z"];
		size = size * 100.f;

		//Vector3f center;
		//center.x = box["center"]["x"];
		//center.y = box["center"]["y"];
		//center.z = box["center"]["z"];
		//center = center * 100.0f;

		collider.extents = size / 2.0f;

		if (box["isDynamic"]) { continue; }

		//Every collider that loads is default Collider type, if u want trigger Add EventComponent in Unity
		//collisionData.type = eCollisionType::Collider;


		aPhysXSceneManager.CreateStatic(aWorld->GetComponent<TransformComponent>(entity).transform, collider.extents);
	}
}

void UnityImporter::LoadCameraComponent(const Json& aJson, World* aWorld)
{

	for (auto& cameraComponent : aJson["cameraComponent"])
	{
		Entity id = unityToEntity.at(cameraComponent["entityID"]);
		aWorld->AddComponent<CameraComponent>(id);

	}
}

void UnityImporter::LoadPlayerComponents(const Json& aJson, World* aWorld, PhysXSceneManager& aPhysXSceneManager)
{
	if (aJson["playerComponent"].size() == 0)
	{
		PrintW("[UnityImporter.cpp] No player exists in Scene!");
		aWorld->SetPlayerEntityID(INVALID_ENTITY);
		return;
	}

	Entity player;
	for (auto& entityID : aJson["playerComponent"])
	{
		player = unityToEntity.at(entityID);
		aWorld->SetPlayerEntityID(player);
		auto& playerComponent = aWorld->AddComponent<PlayerComponent>(player);
		auto& transformComponent = aWorld->GetComponent<TransformComponent>(player);
		aWorld->AddComponent<ColliderComponent>(player);
		aWorld->AddComponent<CollisionDataComponent>(player);

		playerComponent.controller = aPhysXSceneManager.CreateCharacterController(transformComponent.transform, &playerComponent.callbackWrapper);
	}
}
void UnityImporter::LoadAnimationComponent(const Json& aJson, World* aWorld)
{
	if (!aJson.contains("animationComponents")) { return; }

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
	for (auto& e : aJson["enemyComponents"])
	{
		Entity entity = unityToEntity.at(e["entityID"]);
		aWorld->AddComponent<EnemyComponent>(entity);
	}
}

void UnityImporter::LoadEventComponent(const Json& aJson, World* aWorld)
{
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

void UnityImporter::LoadDirectionalLightComponent(const Json& aJson, World* aWorld)
{
	aJson;
	aWorld;
	for (auto& dirLight : aJson["directionalLightComponentData"])
	{
		//Entity entity = unityToEntity.at(dirLight["entityID"]);


		//auto transform = aWorld->GetComponent<TransformComponent>(entity);
		Vector3f direction;// = transform->transform.GetForward();

		direction.x = dirLight["angle"]["x"];
		direction.y = dirLight["angle"]["y"];
		direction.z = dirLight["angle"]["z"];

		Vector4f color = {
			color.x = dirLight["color"]["x"],
			color.y = dirLight["color"]["y"],
			color.z = dirLight["color"]["z"],
			1.0f
		};

		//GraphicsEngine::GetInstance()->GetDirectionalLightManager()->Init(direction, color);
	}
}


void UnityImporter::LoadPhysXComponent(const Json& aJson, World* aWorld, PhysXSceneManager& aPhysXSceneManager)
{
	using namespace physx;
	for (auto& physX : aJson["physXComponents"])
	{
		Entity entity = unityToEntity.at(physX["entityID"]);
		auto& physXComponent = aWorld->AddComponent<PhysXComponent>(entity);
		auto& collider = aWorld->GetComponent<ColliderComponent>(entity);
		//physXComponent.dynamicPhysX = cc->getActor();
		collider;
		physXComponent.dynamicPhysX = aPhysXSceneManager.CreateDynamicBox(aWorld->GetComponent<TransformComponent>(entity).transform, collider.extents);
	}
}

void UnityImporter::LoadOrbComponent(const Json& aJson, World* aWorld)
{
	for (auto& orb : aJson["orbComponent"])
	{
		Entity id = unityToEntity.at(orb["entityID"]);
		aWorld->AddComponent<OrbComponent>(id);
	}
}

void UnityImporter::LoadProjectileComponent(const Json& aJson, World* aWorld)
{
	for (auto& orb : aJson["projectileComponents"])
	{
		Entity id = unityToEntity.at(orb["entityID"]);
		aWorld->AddComponent<ProjectileComponent>(id);
		ProjectileFactory::GetInstance().SetMeshID(id, aWorld);
	}
}

void UnityImporter::LoadNpcComponent(const Json& aJson, World* aWorld)
{


	for (auto& npc : aJson["nonPlayerComponent"])
	{
		Entity entity = unityToEntity.at(npc["entityID"]);
		auto& npccomp = aWorld->AddComponent<NpcComponent>(entity);

		npccomp.ID = npc["myNPCID"];
		npccomp.GroupID = npc["myGroupID"];
		npccomp.DelayTimer = npc["myStartPathDelayTimer"];
		npccomp.IsActive = npc["myIsActive"];



	}
}

Vector3f UnityImporter::GetVector3f(const Json& aJson)
{
	float x = aJson["x"].get<float>();
	float y = aJson["y"].get<float>();
	float z = aJson["z"].get<float>();
	return Vector3f{ x, y, z };
}