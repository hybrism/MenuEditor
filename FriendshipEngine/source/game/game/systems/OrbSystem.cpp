#include "pch.h"
#include "OrbSystem.h"

#include <ecs/World.h>
#include <nlohmann/json.hpp>
#include <array>

#include "../component/OrbComponent.h"
#include "../component/PlayerComponent.h"
#include "../component/TransformComponent.h"
#include "engine\debug\DebugLine.h"

#include <engine/utility/InputManager.h>
#include "../utility/JsonUtility.h"

OrbSystem::OrbSystem(World* aWorld) : System(aWorld)
{
	DebugLine line({}, {}, {1,0,0,1});
	myFollowLines.resize(mylength, line);
	for (size_t i = 0; i < myFollowLines.size(); i++)
	{
		myFollowLines[i].SetColor({1,1,0,1});
	}
}

OrbSystem::~OrbSystem()
{
}

void OrbSystem::Init()
{
	for (Entity entity : myEntities)
	{
		OrbComponent& orbComponent = myWorld->GetComponent<OrbComponent>(entity);
		TransformComponent& transform = myWorld->GetComponent<TransformComponent>(entity);
		transform.transform.SetScale(Vector3f{ 0.1f,0.1f,0.1f });
		LoadInOrbData(ORB_DATA, orbComponent);
	}
}

void OrbSystem::Update(const float& dt)
{
	for (Entity entity : myEntities)
	{
		auto playerID = myWorld->GetPlayerEntityID();
		auto& playerComp = myWorld->GetComponent<PlayerComponent>(playerID);
		myPlayerPos = Vector3f{(float)playerComp.controller->getPosition().x,
							   (float)playerComp.controller->getPosition().y,
							   (float)playerComp.controller->getPosition().z };

		TransformComponent& transformComponent = myWorld->GetComponent<TransformComponent>(entity);
		OrbComponent& orbComponent = myWorld->GetComponent<OrbComponent>(entity);

		if (orbComponent.playerHealth > 0)
		{
			CreateFollowPath(playerComp, dt);
			FollowPath(playerComp,transformComponent, orbComponent, dt);
			PlayerRegainHealth(orbComponent, dt);
		}

		orbComponent.OrbPos = transformComponent.transform.GetPosition();
	}
}

void OrbSystem::Render()
{
#ifdef _DEBUG	
	if (myFollowPath.size() >= 2) // Ensure at least two points for drawing lines
	{
		// Create DebugLine objects based on myFollowPath
		for (int i = 0; i < myFollowPath.size() - 1; ++i)
		{
			myFollowLines[i].SetStartPosition(myFollowPath[i]);
			myFollowLines[i].SetEndPosition(myFollowPath[i + 1]);
		}

		//Initialize and render the lines
		for (int i = 0; i < myFollowLines.size(); i++)
		{
			myFollowLines[i].DrawLine();
		}
	}
#endif
}

void OrbSystem::LoadInOrbData(std::string aFile, OrbComponent& aOrb)
{

	nlohmann::json jsonData = JsonUtility::OpenJson(aFile);

	nlohmann::json orbSettingsData = jsonData["OrbSettings"];

	for (int i = 0; i < orbSettingsData.size(); i++)
	{
		nlohmann::json orbSpeedSettings = orbSettingsData[i]["OrbSpeedSetting"];

		aOrb.ChaseSpeed = orbSpeedSettings["ChasingSpeed"];
		//aOrb.CatchingSpeed = orbSpeedSettings["CatchingSpeed"];
		//aOrb.RegainSpeed = orbSpeedSettings["RegainSpeed"];
	}
}

void OrbSystem::CreateFollowPath(PlayerComponent& aPlayer, const float aDT)
{
	if (aPlayer.finalVelocity.Length() > 1.0f )
	{
		myCreationTimer -= aDT;

		if (myCreationTimer <= 0)
		{
			myCreationTimer = .1f;

			if (myFollowPath.size() == mylength)
			{
				myFollowPath.erase(myFollowPath.begin());
			}

			myFollowPath.push_back(myPlayerPos);
		}
	}
}

void OrbSystem::FollowPath(PlayerComponent& aPlayer,TransformComponent& atransform, OrbComponent& aOrb, const float aDT)
{
	
	float distanceToPlayer = 0;
	if (myFollowPath.size() > 0)
	{
		for (int i = myPathIndex; i < myFollowPath.size() - 1; i++)
		{
			if (i < myFollowPath.size())
			{
				distanceToPlayer += Vector3f::Distance(myFollowPath[i], myFollowPath[i + 1]);
			}
			else
			{
				distanceToPlayer += Vector3f::Distance(myFollowPath[i], myPlayerPos);
			}
		}
	}

	//Limit orb movement if speed is maintained
	if (aPlayer.finalVelocity.Length() > 10.0f)
	{
		myChaseTimer = 0;
		myPathIndex = 0;
	}

	if (myFollowPath.size() > 1)
	{
		//move
		atransform.transform.SetPosition(myFollowPath[myPathIndex]);

		myChaseTimer += aDT;

		if (myChaseTimer > myChaseSpeedThreshold)
		{
			myPathIndex++;
			myChaseTimer = 0;

			if (myPathIndex == myFollowPath.size())
			{
				atransform.transform.SetPosition(myFollowPath.front());
				myPathIndex = 0;

				//Take damage
				PlayerTakeDamage(aOrb);
			}
		}
	}
}

void OrbSystem::PlayerRegainHealth(OrbComponent& aOrb, const float& dt)
{
	if (aOrb.playerHealth < aOrb.playerMaxHealth)
	{
		aOrb.HPRegainTimer -= dt;

		if (aOrb.HPRegainTimer <= 0)
		{
			aOrb.playerHealth++;
			std::cout << "Health +1" << std::endl;
		}
	}
}

void OrbSystem::PlayerTakeDamage(OrbComponent& aOrb)
{
	aOrb.playerHealth--;
	std::cout << "Damage taken" << std::endl;

	//player died
	if (aOrb.playerHealth <= 0)
	{
		std::cout << "player killed" << std::endl;
	}
	else
	{
		aOrb.HPRegainTimer = aOrb.HPRegaintime;
	}
}
