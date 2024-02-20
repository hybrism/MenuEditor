#pragma once
#include "../windows/Window.h"
#include <ecs/entity/Entity.h>

class World;
class Texture;

namespace FE
{
	class Inspector : public WindowBase
	{
	public:
		Inspector(Entity aEntity, int aWindowID);

		void Show(const EditorUpdateContext& aContext) override;

		Entity GetEntityID() const { return mySelectedEntity; }
		int GetWindowID() const { return myWindowID; }
		void SetEntityID(const Entity& aEntity);

	private:
		void Header(const std::string& aTitle);

		void DisplayMetaData(World* aWorld);
		void DisplayTransformData(World* aWorld);
		void DisplayMeshAndTextureData(World* aWorld);
		void DisplayColliderData(World* aWorld);
		void DisplayPlayerData(World* aWorld);
		void DisplayEnemyData(World* aWorld);
		void DisplayScriptableEventData(World* aWorld);
		void DisplayOrbData(World* aWorld);

		Texture* myTextureToDisplay;
		Entity mySelectedEntity;
		int myWindowID;
		bool myTextureWindowOpen;

	};
};