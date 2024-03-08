#include "MenuHandler.h"
#include <fstream>
#include <ostream>
#include <engine/Paths.h>
#include <engine/utility/Error.h>
#include <engine/graphics/GraphicsEngine.h>
#include <assets/TextureFactory.h>

#include <nlohmann/json.hpp>
#include <shared/postMaster/PostMaster.h>

#include "MenuObject.h"

#include "../MenuEditorCommon.h"
#include "components/SpriteComponent.h"
#include "components/TextComponent.h"
#include "components/Collider2DComponent.h"
#include "components/CommandComponent.h"

MENU::MenuHandler::MenuHandler()
{}

MENU::MenuHandler::~MenuHandler()
{}

void MENU::MenuHandler::Init(const std::string& aMenuFile)
{
	//TODO:  ViewPort != RenderSize, add get RenderSize to graphicsengine 23.f is the height of menubar?
	Vector2i viewport = GraphicsEngine::GetInstance()->GetViewportDimensions();
	myRenderSize = { (float)viewport.x, (float)viewport.y + 23.f };

	LoadFromJson(aMenuFile);
}

void MENU::MenuHandler::Update(const MenuUpdateContext& aContext)
{
	if (myStateStack.empty())
		return;

	for (ID id : myStateStack.top()->objectIds)
	{
		MenuObject& currentObj = myObjectManager.GetObjectFromID(id);
		currentObj.Update(aContext);

		if (currentObj.HasComponent<CommandComponent>())
		{
			if (!currentObj.IsPressed())
				return;

			auto& command = currentObj.GetComponent<CommandComponent>();
			switch (command.GetCommand())
			{
			case eCommandType::PopMenu:
				PrintI("Go back to previous menu");
				//PopState();
				break;
			case eCommandType::PushSettingsMenu:
				PrintI("Open Settings menu");
				//PushState(1);
				break;
			case eCommandType::PushLevelSelectMenu:
				PrintI("Open Level Select menu");
				//PushState(2);
				break;
			case eCommandType::LoadLevel3:
				PrintI("Load Level 3!");
				break;
			case eCommandType::LoadFeatureGym:
				PrintI("Load FeatureGym!");
				break;
			case eCommandType::QuitGame:
				PrintI("Quit Game!");
				break;
			}
		}

	}
}

void MENU::MenuHandler::Render()
{
	if (myStateStack.empty())
		return;

	for (ID id : myStateStack.top()->objectIds)
	{
		myObjectManager.GetObjectFromID(id).Render();
	}
}

void MENU::MenuHandler::AddNewState(const std::string& aName)
{
	MenuState newState;

	newState.name = aName;
	newState.id = IDManager::GetInstance()->GetFreeID();

	myStates.push_back(newState); 
}

void MENU::MenuHandler::PushState(ID aID)
{
	for (size_t i = 0; i < myStates.size(); i++)
	{
		if (myStates[i].id == aID)
			myStateStack.push(&myStates[i]);
	}
}

void MENU::MenuHandler::PopState()
{
	if (myStateStack.empty())
		return;

	myStateStack.pop();
}

MENU::MenuObject& MENU::MenuHandler::GetObjectFromID(ID aID)
{
	return myObjectManager.GetObjectFromID(aID);
}

MENU::MenuObject& MENU::MenuHandler::GetObjectFromIndex(ID aIndex)
{
	return myObjectManager.GetObjectFromID(GetCurrentState().objectIds[aIndex]);
}

MENU::MenuObject& MENU::MenuHandler::CreateNewObject(const Vector2f& aPosition)
{
	ID id = IDManager::GetInstance()->GetFreeID();
	MenuObject& newObject = myObjectManager.CreateNew(id, aPosition);
	GetCurrentState().objectIds.push_back(id);

	return newObject;
}

MENU::MenuState& MENU::MenuHandler::GetCurrentState()
{
	assert(!myStateStack.empty() && "StateStack is empty!");

	return *myStateStack.top();
}

std::vector<MENU::MenuState>& MENU::MenuHandler::GetAllStates()
{
	return myStates;
}

void MENU::MenuHandler::RemoveObjectAtID(ID aID)
{
	for (size_t i = 0; i < myStates.size(); i++)
	{
		for (size_t j = 0; j < myStates[i].objectIds.size(); j++)
		{
			if (myStates[i].objectIds[j] != aID)
				continue;

			myStates[i].objectIds.erase(myStates[i].objectIds.begin() + j);
			continue;
		}
	}

	myObjectManager.RemoveObjectAtID(aID);
	IDManager::GetInstance()->FreeID(aID);
}

void MENU::MenuHandler::MoveUpObjectAtID(ID aID)
{
	for (size_t indexOfObjectToMove = 0; indexOfObjectToMove < GetCurrentState().objectIds.size(); indexOfObjectToMove++)
	{
		if (aID != GetCurrentState().objectIds[indexOfObjectToMove])
			continue;

		if (indexOfObjectToMove == 0)
			return;

		ID temp = GetCurrentState().objectIds[indexOfObjectToMove - 1];
		GetCurrentState().objectIds[indexOfObjectToMove - 1] = GetCurrentState().objectIds[indexOfObjectToMove];
		GetCurrentState().objectIds[indexOfObjectToMove] = temp;
	}
}

void MENU::MenuHandler::MoveDownObjectAtID(ID aID)
{
	for (size_t indexOfObjectToMove = 0; indexOfObjectToMove < GetCurrentState().objectIds.size(); indexOfObjectToMove++)
	{
		if (aID != GetCurrentState().objectIds[indexOfObjectToMove])
			continue;

		if (indexOfObjectToMove == GetCurrentState().objectIds.size() - 1)
			return;

		ID temp = GetCurrentState().objectIds[indexOfObjectToMove + 1];
		GetCurrentState().objectIds[indexOfObjectToMove + 1] = GetCurrentState().objectIds[indexOfObjectToMove];
		GetCurrentState().objectIds[indexOfObjectToMove] = temp;
	}
}

void MENU::MenuHandler::LoadFromJson(const std::string& aMenuFile)
{
	IDManager* idManager = IDManager::GetInstance();

	myObjectManager.ClearAll();
	myStates.clear();

	while (!myStateStack.empty())
		myStateStack.pop();

	myFileName = aMenuFile;
	myName = aMenuFile.substr(0, aMenuFile.find_last_of('.'));

	std::string MENU_PATH = "menus/";
	std::string path = RELATIVE_IMPORT_DATA_PATH + MENU_PATH + myFileName;
	std::ifstream dataFile(path);
	if (dataFile.fail())
	{
		PrintE("Could not open \"" + path + "\" to read data");
		return;
	}

	nlohmann::json menuFile = nlohmann::json::parse(dataFile);
	dataFile.close();

	for (size_t i = 0; i < menuFile["menuState"].size(); i++)
	{
		MenuState newState;
		newState.name = menuFile["menuState"][i]["name"];
		newState.id = idManager->UseID(menuFile["menuState"][i]["ID"]);
		for (size_t j = 0; j < menuFile["menuState"][i]["objects"].size(); j++)
		{
			newState.objectIds.push_back(menuFile["menuState"][i]["objects"][j]);
		}
		myStates.push_back(newState);
	}

	for (size_t i = 0; i < menuFile["objectID"].size(); i++)
	{
		ID id = menuFile["objectID"][i]["objectID"];
		MenuObject& obj = myObjectManager.CreateNew(id);
		obj.SetName(menuFile["objectID"][i]["name"]);
		obj.SetPosition(JsonToScreenPosition(menuFile["objectID"][i]["position"]));
	}

	nlohmann::json spriteComponents = menuFile["spriteComponents"];
	for (size_t i = 0; i < spriteComponents.size(); i++)
	{
		size_t ownerID = spriteComponents[i]["ownerID"];
		SpriteComponent& sprite = myObjectManager.myObjects[ownerID]->AddComponent<SpriteComponent>();

		//TODO: AssetDatabase for sprites?
		nlohmann::json textures = spriteComponents[i]["textures"];
		for (size_t textureStateIndex = 0; textureStateIndex < textures.size(); textureStateIndex++)
		{
			std::string texturePath = textures[textureStateIndex]["path"];

			if (texturePath == "(None)")
				continue;

			sprite.SetTexture(TextureFactory::CreateTexture(RELATIVE_SPRITE_ASSET_PATH + texturePath, false), texturePath, (TextureState)textureStateIndex);
			sprite.SetColor(JsonToColorVec(textures[textureStateIndex]["color"]), (TextureState)textureStateIndex);
		}

		sprite.SetPosition(JsonToScreenPosition(spriteComponents[i]["position"]));
		sprite.SetSize(JsonToVec2(spriteComponents[i]["size"]));
		sprite.SetPivot(JsonToVec2(spriteComponents[i]["pivot"]));
		sprite.SetScaleMultiplier(JsonToVec2(spriteComponents[i]["scaleMultiplier"]));

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
		text.SetPosition(JsonToScreenPosition(textComponents[i]["position"]));
		text.SetColor(JsonToColorVec(textComponents[i]["color"]));
		text.SetIsCentered(textComponents[i]["isCentered"]);

	}

	nlohmann::json colliderComponents = menuFile["colliderComponents"];
	for (size_t i = 0; i < colliderComponents.size(); i++)
	{
		size_t ownerID = colliderComponents[i]["ownerID"];
		Collider2DComponent& collider = myObjectManager.myObjects[ownerID]->AddComponent<Collider2DComponent>();

		collider.SetPosition(JsonToScreenPosition(colliderComponents[i]["position"]));
		collider.SetSize(JsonToVec2(colliderComponents[i]["size"]));
	}

	nlohmann::json commandComponents = menuFile["commandComponents"];
	for (size_t i = 0; i < commandComponents.size(); i++)
	{
		size_t ownerID = commandComponents[i]["ownerID"];
		CommandComponent& command = myObjectManager.myObjects[ownerID]->AddComponent<CommandComponent>();
		command.SetCommand((eCommandType)commandComponents[i]["command"]);
	}

	if (myStates.empty())
	{
		MenuState newState;
		newState.name = "Base";
		newState.id = idManager->GetFreeID();

		myStates.push_back(newState);
	}

	myStateStack.push(&myStates.front());
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

Vector2f MENU::MenuHandler::JsonToScreenPosition(nlohmann::json aJson)
{
	Vector2f screenPosition = JsonToVec2(aJson);

	screenPosition.x *= myRenderSize.x;
	screenPosition.y *= myRenderSize.y;

	return screenPosition;
}

void MENU::MenuHandler::SaveToJson()
{
	nlohmann::json menuFile;
	menuFile["name"] = myName;

	nlohmann::json states;
	for (size_t i = 0; i < myStates.size(); i++)
	{
		nlohmann::json state;
		state["name"] = myStates[i].name;
		state["ID"] = myStates[i].id;

		for (size_t j = 0; j < myStates[i].objectIds.size(); j++)
			state["objects"].push_back(myStates[i].objectIds[j]);

		states.push_back(state);
	}

	menuFile["menuState"] = states;

	nlohmann::json objectIds;
	for (size_t i = 0; i < myObjectManager.myObjects.size(); i++)
	{
		MenuObject& object = *myObjectManager.myObjects[i];
		nlohmann::json objectData;

		objectData["objectID"] = object.GetID();
		objectData["name"] = object.GetName();
		objectData["position"] = ScreenPositionToJson(object.GetPosition());
		objectIds.push_back(objectData);
	}

	menuFile["objectID"] = objectIds;

	nlohmann::json spriteComponents;
	nlohmann::json textComponents;
	nlohmann::json colliderComponents;
	nlohmann::json commandComponents;

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

				for (size_t textureStateIndex = 0; textureStateIndex < (int)TextureState::Count; textureStateIndex++)
				{
					nlohmann::json textureEntry;
					textureEntry["path"] = sprite.GetTexturePath((TextureState)textureStateIndex);
					textureEntry["color"] = ColorVecToJson(sprite.GetColor((TextureState)textureStateIndex));

					spriteEntry["textures"].push_back(textureEntry);
				}

				spriteEntry["position"] = ScreenPositionToJson(sprite.GetPosition());
				spriteEntry["size"] = Vec2ToJson(sprite.GetSize());
				spriteEntry["pivot"] = Vec2ToJson(sprite.GetPivot());
				spriteEntry["scaleMultiplier"] = Vec2ToJson(sprite.GetScaleMultiplier());
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
			textEntry["position"] = ScreenPositionToJson(text.GetPosition());
			textEntry["font"] = text.GetFontName();
			textEntry["fontSize"] = (int)text.GetFontSize();
			textEntry["isCentered"] = text.GetIsCentered();
			textEntry["color"] = ColorVecToJson(text.GetColor());
			textComponents.push_back(textEntry);
		}

		if (myObjectManager.myObjects[i]->HasComponent<Collider2DComponent>())
		{
			Collider2DComponent collider = myObjectManager.myObjects[i]->GetComponent<Collider2DComponent>();
			nlohmann::json colliderEntry;
			colliderEntry["ownerID"] = i;
			colliderEntry["position"] = ScreenPositionToJson(collider.GetPosition());
			colliderEntry["size"] = Vec2ToJson(collider.GetSize());
			colliderComponents.push_back(colliderEntry);
		}

		if (myObjectManager.myObjects[i]->HasComponent<CommandComponent>())
		{
			CommandComponent command = myObjectManager.myObjects[i]->GetComponent<CommandComponent>();
			nlohmann::json commandEntry;
			commandEntry["ownerID"] = i;
			commandEntry["command"] = (int)command.GetCommand();
			commandComponents.push_back(commandEntry);
		}
	}

	menuFile["spriteComponents"] = spriteComponents;
	menuFile["textComponents"] = textComponents;
	menuFile["colliderComponents"] = colliderComponents;
	menuFile["commandComponents"] = commandComponents;

	std::string MENU_PATH = "menus/";
	std::string path = RELATIVE_IMPORT_DATA_PATH + MENU_PATH + myFileName;
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

nlohmann::json MENU::MenuHandler::ScreenPositionToJson(const Vector2f& aPosition)
{
	Vector2f screenPos = { aPosition.x / myRenderSize.x, aPosition.y / myRenderSize.y };
	return Vec2ToJson(screenPos);
}
