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
	myFollowLines.resize(mySize, line);
	for (size_t i = 0; i < myFollowLines.size(); i++)
	{
		myFollowLines[i].Init();
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

		CreateFollowPath(playerComp, dt);
		FollowPath(transformComponent, dt);

#ifdef _DEBUG	
		//DebugLine line = DebugLine(Vector3f(transformComponent.transform.GetPosition().x, transformComponent.transform.GetPosition().y, transformComponent.transform.GetPosition().z),
		//	                       Vector3f(transformComponent.transform.GetPosition().x, transformComponent.transform.GetPosition().y + 100.f, transformComponent.transform.GetPosition().z));

		////Orb
		//line.Init();
		//line.SetColor(Vector4(1, 0, 0, 1));
		//line.DrawLine();
#endif
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

	/*DebugLine line = DebugLine(Vector3(myPlayerPos.x, myPlayerPos.y, myPlayerPos.z),
		Vector3f(myPlayerPos.x, myPlayerPos.y + 100.f, myPlayerPos.z));

	line.Init();
	line.SetColor(Vector4(1, 0, 0, 1));
	line.DrawLine();*/

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
		aOrb.CatchingSpeed = orbSpeedSettings["CatchingSpeed"];
		aOrb.RegainSpeed = orbSpeedSettings["RegainSpeed"];
	}
}

void OrbSystem::CreateFollowPath(PlayerComponent& aPlayer, const float aDT)
{
	if (aPlayer.finalVelocity.Length() > 0 )
	{
		myCreationTimer -= aDT;

		if (myCreationTimer <= 0)
		{
			myCreationTimer = .1f;

			if (myFollowPath.size() == mySize)
			{
				myFollowPath.erase(myFollowPath.begin());
			}

			myFollowPath.push_back(myPlayerPos);
		}
	}
}

void OrbSystem::FollowPath(TransformComponent& atransform, const float aDT)
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

	if (myFollowPath.size() > 1)
	{
		//move
		atransform.transform.SetPosition(myFollowPath[myPathIndex]);

		myChaseTimer += aDT;

		if (myChaseTimer > 0.5f)
		{
			myPathIndex++;
			myChaseTimer = 0;

			if (myPathIndex == myFollowPath.size())
			{
				atransform.transform.SetPosition(myFollowPath.front());
				myPathIndex = 0;
			}
		}
	}
}