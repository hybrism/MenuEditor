#include "MenuHandler.h"
#include <fstream>
#include <ostream>
#include <engine/utility/Error.h>

#include <nlohmann/json.hpp>
#include <assets/TextureFactory.h>

#include <shared/postMaster/PostMaster.h>

#include "MenuObject.h"
#include "../MenuCommon.h"
#include "components/SpriteComponent.h"
#include "components/TextComponent.h"
#include "components/Collider2DComponent.h"

MENU::MenuHandler::MenuHandler()
{}

MENU::MenuHandler::~MenuHandler()
{}

void MENU::MenuHandler::Init(const std::string& aMenuFile, TextureFactory* aTextureFactory)
{
	LoadFromJson(aMenuFile, aTextureFactory);
}

void MENU::MenuHandler::Update()
{
	myObjectManager.Update();
}

void MENU::MenuHandler::Render()
{
	myObjectManager.Render();
}

MENU::MenuObject& MENU::MenuHandler::GetObjectFromID(size_t aID)
{
	return myObjectManager.GetObjectFromID(aID);
}

void MENU::MenuHandler::CreateNewObject(const Vector2f& aPosition)
{
	myObjectManager.CreateNew(aPosition);
}

size_t MENU::MenuHandler::GetObjectsSize()
{
	return myObjectManager.myObjects.size();
}

void MENU::MenuHandler::LoadFromJson(const std::string& aMenuFile, TextureFactory* aTextureFactory)
{
	myObjectManager.ClearAll();

	myFileName = aMenuFile;
	myName = aMenuFile.substr(0, aMenuFile.find_last_of('.'));

	std::string path = MENU::RELATIVE_MENUEDITOR_ASSETS + MENU::MENU_PATH + myFileName;
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
		obj.SetPosition(JsonToVec2(menuFile["objectID"][i]["position"]));
	}

	nlohmann::json spriteComponents = menuFile["spriteComponents"];
	for (size_t i = 0; i < spriteComponents.size(); i++)
	{
		size_t ownerID = spriteComponents[i]["ownerID"];
		SpriteComponent& sprite = myObjectManager.myObjects[ownerID]->AddComponent<SpriteComponent>();

		//TODO: AssetDatabase for sprites?
		std::string spriteAssetPath = MENU::RELATIVE_MENUEDITOR_ASSETS + MENU::SPRITES;
		std::string texturePath = spriteComponents[i]["texture"];
		sprite.SetTexture(aTextureFactory->CreateTexture(spriteAssetPath + texturePath, false), texturePath);

		sprite.SetPosition(JsonToVec2(spriteComponents[i]["position"]));
		sprite.SetSize(JsonToVec2(spriteComponents[i]["size"]));
		sprite.SetPivot(JsonToVec2(spriteComponents[i]["pivot"]));
		sprite.SetScaleMultiplier(JsonToVec2(spriteComponents[i]["scaleMultiplier"]));
		sprite.SetColor(JsonToColorVec(spriteComponents[i]["color"]));

		ClipValue clip;
		clip.left = spriteComponents[i]["clip"]["left"];
		clip.right = spriteComponents[i]["clip"]["right"];
		clip.down = spriteComponents[i]["clip"]["down"];
		clip.upper = spriteComponents[i]["clip"]["upper"];
		sprite.SetClipValue(clip);

		sprite.SetRotation(spriteComponents[i]["rotation"]);
		sprite.SetIsHidden(spriteComponents[i]["isHidden"]);
	}

	nlohmann::json textComponents = menuFile["textComponents"];
	for (size_t i = 0; i < textComponents.size(); i++)
	{
		size_t ownerID = textComponents[i]["ownerID"];
		TextComponent& text = myObjectManager.myObjects[ownerID]->AddComponent<TextComponent>();
		text.SetFont(textComponents[i]["font"]);
		text.SetFontSize((eSize)textComponents[i]["fontSize"]);
		text.SetText(textComponents[i]["textString"]);
		text.SetPosition(JsonToVec2(textComponents[i]["position"]));
		text.SetIsCentered(textComponents[i]["isCentered"]);

	}

	nlohmann::json colliderComponents = menuFile["colliderComponents"];
	for (size_t i = 0; i < colliderComponents.size(); i++)
	{
		size_t ownerID = colliderComponents[i]["ownerID"];
		Collider2DComponent& collider = myObjectManager.myObjects[ownerID]->AddComponent<Collider2DComponent>();

		collider.SetPosition(JsonToVec2(colliderComponents[i]["position"]));
		collider.SetSize(JsonToVec2(colliderComponents[i]["size"]));

	}
}

Vector4f MENU::MenuHandler::JsonToColorVec(nlohmann::json aJson)
{
	Vector4f vec;
	vec.x = aJson["r"];
	vec.y = aJson["g"];
	vec.z = aJson["b"];
	vec.w = aJson["a"];

	return vec;
}

Vector2f MENU::MenuHandler::JsonToVec2(nlohmann::json aJson)
{
	Vector2f vec;
	vec.x = aJson["x"];
	vec.y = aJson["y"];

	return vec;
}

void MENU::MenuHandler::SaveToJson()
{
	nlohmann::json menuFile;
	menuFile["name"] = myName;

	nlohmann::json objects;
	for (size_t i = 0; i < myObjectManager.myObjects.size(); i++)
	{
		MenuObject& object = *myObjectManager.myObjects[i];
		nlohmann::json objectData;

		objectData["objectID"] = object.GetID();
		objectData["name"] = object.GetName();
		objectData["position"] = Vec2ToJson(object.GetPosition());
		objects.push_back(objectData);
	}

	menuFile["objectID"] = objects;

	nlohmann::json spriteComponents;
	nlohmann::json textComponents;
	nlohmann::json colliderComponents;

	for (size_t i = 0; i < myObjectManager.myObjects.size(); i++)
	{
		if (myObjectManager.myObjects[i]->HasComponent<SpriteComponent>())
		{
			auto sprites = myObjectManager.myObjects[i]->GetComponents<SpriteComponent>();
			for (int componentIndex = 0; componentIndex < sprites.size(); componentIndex++)
			{
				SpriteComponent& sprite = static_cast<SpriteComponent&>(*sprites[componentIndex]);
				nlohmann::json spriteEntry;
				spriteEntry["ownerID"] = i;
				spriteEntry["texture"] = sprite.GetTexturePath();
				spriteEntry["position"] = Vec2ToJson(sprite.GetPosition());
				spriteEntry["size"] = Vec2ToJson(sprite.GetSize());
				spriteEntry["pivot"] = Vec2ToJson(sprite.GetPivot());
				spriteEntry["scaleMultiplier"] = Vec2ToJson(sprite.GetScaleMultiplier());
				spriteEntry["color"] = ColorVecToJson(sprite.GetColor());
				spriteEntry["clip"]["left"] = sprite.GetClipValue().left;
				spriteEntry["clip"]["right"] = sprite.GetClipValue().right;
				spriteEntry["clip"]["down"] = sprite.GetClipValue().down;
				spriteEntry["clip"]["upper"] = sprite.GetClipValue().upper;
				spriteEntry["rotation"] = sprite.GetRotation();
				spriteEntry["isHidden"] = sprite.GetIsHidden();
				spriteComponents.push_back(spriteEntry);
			}
		}

		if (myObjectManager.myObjects[i]->HasComponent<TextComponent>())
		{
			TextComponent text = myObjectManager.myObjects[i]->GetComponent<TextComponent>();
			nlohmann::json textEntry;
			textEntry["ownerID"] = i;
			textEntry["textString"] = text.GetText();
			textEntry["position"] = Vec2ToJson(text.GetPosition());
			textEntry["font"] = text.GetFontName();
			textEntry["fontSize"] = (int)text.GetFontSize();
			textEntry["isCentered"] = text.GetIsCentered();
			textComponents.push_back(textEntry);
		}

		if (myObjectManager.myObjects[i]->HasComponent<Collider2DComponent>())
		{
			Collider2DComponent collider = myObjectManager.myObjects[i]->GetComponent<Collider2DComponent>();
			nlohmann::json colliderEntry;
			colliderEntry["ownerID"] = i;
			colliderEntry["position"] = Vec2ToJson(collider.GetPosition());
			colliderEntry["size"] = Vec2ToJson(collider.GetSize());
			colliderComponents.push_back(colliderEntry);
		}
	}

	menuFile["spriteComponents"] = spriteComponents;
	menuFile["textComponents"] = textComponents;
	menuFile["colliderComponents"] = colliderComponents;


	std::string path = MENU::RELATIVE_MENUEDITOR_ASSETS + MENU::MENU_PATH + myFileName;
	std::ofstream dataFile(path);
	if (dataFile.fail())
	{
		PrintE("Could not open \"" + path + "\" to save data");
		return;
	}

	dataFile << menuFile;

	dataFile.close();

}

nlohmann::json MENU::MenuHandler::ColorVecToJson(const Vector4f& aVec)
{
	nlohmann::json vec;
	vec["r"] = aVec.x;
	vec["g"] = aVec.y;
	vec["b"] = aVec.z;
	vec["a"] = aVec.w;

	return vec;
}

nlohmann::json MENU::MenuHandler::Vec2ToJson(const Vector2f& aVec)
{
	nlohmann::json vec;
	vec["x"] = aVec.x;
	vec["y"] = aVec.y;

	return vec;
}
