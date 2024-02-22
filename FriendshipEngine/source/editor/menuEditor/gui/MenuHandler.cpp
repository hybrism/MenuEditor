#include "MenuHandler.h"

#include <fstream>
#include <iostream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <engine/utility/Error.h>
#include <assets/TextureFactory.h>
#include <shared/postMaster/PostMaster.h>

#include "../MenuCommon.h"

#include "menuObject/MenuObject.h"

#include "components/SpriteComponent.h"
#include "components/TextComponent.h"
#include "components/Collider2DComponent.h"

MenuHandler::MenuHandler()
{
}

MenuHandler::~MenuHandler()
{
}

void MenuHandler::Init(const std::string& aMenuFile, TextureFactory* aTextureFactory)
{
	LoadFromJson(aMenuFile, aTextureFactory);
	FE::PostMaster::GetInstance()->SendMessage({ FE::eMessageType::NewMenuLoaded, aMenuFile });
}

void MenuHandler::Update()
{
	myObjectManager.Update();
}

void MenuHandler::Render()
{
	myObjectManager.Render();
}

void MenuHandler::LoadFromJson(const std::string& aMenuFile, TextureFactory* aTextureFactory)
{
	myObjectManager.ClearAll();

	myFileName = aMenuFile;
	myName = aMenuFile.substr(0, aMenuFile.find_last_of('.'));

	std::string path = ME::RELATIVE_MENUEDITOR_ASSETS + ME::MENU_PATH + myFileName;
	std::ifstream dataFile(path);
	if (dataFile.fail())
	{
		PrintE("Could not open \"" + path + "\" to read data");
		return;
	}

	nlohmann::json menuFile = nlohmann::json::parse(dataFile);
	dataFile.close();

	for (size_t i = 0; i < menuFile["objectID"].size(); i++)
	{
		MenuObject& obj = myObjectManager.CreateNew();
		obj.SetName(menuFile["objectID"][i]["name"]);
	}

	nlohmann::json spriteComponents = menuFile["spriteComponent"];
	for (size_t i = 0; i < spriteComponents.size(); i++)
	{
		size_t ownerID = spriteComponents[i]["ownerID"];
		SpriteComponent& sprite = myObjectManager.myObjects[ownerID].AddComponent<SpriteComponent>();

		//TODO: AssetDatabase for sprites?
		std::string spriteAssetPath = ME::RELATIVE_MENUEDITOR_ASSETS + ME::SPRITES;
		std::string texturePath = spriteComponents[i]["texture"];
		sprite.SetTexture(aTextureFactory->CreateTexture(spriteAssetPath + texturePath, false), texturePath);

		SpriteInstanceData& instanceData = sprite.GetInstanceData();
		instanceData.position.x = spriteComponents[i]["position"]["x"];
		instanceData.position.y = spriteComponents[i]["position"]["y"];
		instanceData.scale.x = spriteComponents[i]["size"]["x"];
		instanceData.scale.y = spriteComponents[i]["size"]["y"];
		instanceData.pivot.x = spriteComponents[i]["pivot"]["x"];
		instanceData.pivot.y = spriteComponents[i]["pivot"]["y"];
		instanceData.scaleMultiplier.x = spriteComponents[i]["scaleMultiplier"]["x"];
		instanceData.scaleMultiplier.y = spriteComponents[i]["scaleMultiplier"]["y"];
		instanceData.color.x = spriteComponents[i]["color"]["r"];
		instanceData.color.y = spriteComponents[i]["color"]["g"];
		instanceData.color.z = spriteComponents[i]["color"]["b"];
		instanceData.color.w = spriteComponents[i]["color"]["a"];
		instanceData.clip.left = spriteComponents[i]["clip"]["left"];
		instanceData.clip.right = spriteComponents[i]["clip"]["right"];
		instanceData.clip.down = spriteComponents[i]["clip"]["down"];
		instanceData.clip.upper = spriteComponents[i]["clip"]["upper"];
		instanceData.rotation = spriteComponents[i]["rotation"];
		instanceData.myIsHidden = spriteComponents[i]["isHidden"];
	}
}

void MenuHandler::SaveToJson()
{
	nlohmann::json menuFile;
	menuFile["name"] = myName;

	nlohmann::json objects;
	for (size_t i = 0; i < myObjectManager.myObjects.size(); i++)
	{
		MenuObject& object = myObjectManager.myObjects[i];
		nlohmann::json objectData;

		objectData["objectID"] = object.myID;
		objectData["name"] = object.GetName();

		objects.push_back(objectData);
	}

	menuFile["objectID"] = objects;

	nlohmann::json spriteComponents;
	for (size_t i = 0; i < myObjectManager.myObjects.size(); i++)
	{
		if (myObjectManager.myObjects[i].HasComponent<SpriteComponent>())
		{
			SpriteComponent sprite = myObjectManager.myObjects[i].GetComponent<SpriteComponent>();
			nlohmann::json spriteEntry;
			spriteEntry["ownerID"] = i;
			spriteEntry["texture"] = sprite.GetTexturePath();
			spriteEntry["position"]["x"] = sprite.GetPosition().x;
			spriteEntry["position"]["y"] = sprite.GetPosition().y;
			spriteEntry["size"]["x"] = sprite.GetSize().x;
			spriteEntry["size"]["y"] = sprite.GetSize().y;
			spriteEntry["pivot"]["x"] = sprite.GetPivot().x;
			spriteEntry["pivot"]["y"] = sprite.GetPivot().y;
			spriteEntry["scaleMultiplier"]["x"] = sprite.GetScaleMultiplier().x;
			spriteEntry["scaleMultiplier"]["y"] = sprite.GetScaleMultiplier().y;
			spriteEntry["color"]["r"] = sprite.GetColor().x;
			spriteEntry["color"]["g"] = sprite.GetColor().y;
			spriteEntry["color"]["b"] = sprite.GetColor().z;
			spriteEntry["color"]["a"] = sprite.GetColor().w;
			spriteEntry["clip"]["left"] = sprite.GetClipValue().left;
			spriteEntry["clip"]["right"] = sprite.GetClipValue().right;
			spriteEntry["clip"]["down"] = sprite.GetClipValue().down;
			spriteEntry["clip"]["upper"] = sprite.GetClipValue().upper;
			spriteEntry["rotation"] = sprite.GetRotation();
			spriteEntry["isHidden"] = sprite.GetIsHidden();
			spriteComponents.push_back(spriteEntry);
		}
	}

	menuFile["spriteComponent"] = spriteComponents;

	std::string path = ME::RELATIVE_MENUEDITOR_ASSETS + ME::MENU_PATH + myFileName;
	std::ofstream dataFile(path);
	if (dataFile.fail())
	{
		PrintE("Could not open \"" + path + "\" to save data");
		return;
	}

	dataFile << menuFile;

	dataFile.close();

}
