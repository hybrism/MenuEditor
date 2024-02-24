#pragma once
#include <string>
#include <vector>

#include <ecs/system/System.h>
#include <engine/math/Vector.h>
#include <shared/postMaster/Observer.h>
#include "Window.h"
#include "../inspector/Inspector.h"

class World;
class Texture;

namespace FE
{
	//class Inspector : public WindowBase
	//{
	//public:
	//	Inspector(Entity aEntity, int aWindowID);

	//	void Show(const WindowUpdateContext& aContext) override;

	//	Entity GetEntityID() const { return mySelectedEntity; }
	//	int GetWindowID() const { return myWindowID; }
	//	void SetEntityID(const Entity& aEntity);

	//private:
	//	void Header(const std::string& aTitle);

	//	void DisplayMetaData(World* aWorld);
	//	void DisplayTransformData(World* aWorld);
	//	void DisplayMeshAndTextureData(World* aWorld);
	//	void DisplayColliderData(World* aWorld);
	//	void DisplayPlayerData(World* aWorld);
	//	void DisplayEnemyData(World* aWorld);
	//	void DisplayScriptableEventData(World* aWorld);
	//	void DisplayOrbData(World* aWorld);

	//	Texture* myTextureToDisplay;
	//	Entity mySelectedEntity;
	//	int myWindowID;
	//	bool myTextureWindowOpen;

	//};

	class InspectorWindow : public WindowBase, public Observer
	{
	public:
		InspectorWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags);

		void Show(const EditorUpdateContext& aContext) override;
		
		void Clear();

	private:
		std::vector<Inspector> myInspectorWindows;
		int myWindowIdCounter = 0;

		void RecieveMessage(const Message& aMessage) override;
	};
}