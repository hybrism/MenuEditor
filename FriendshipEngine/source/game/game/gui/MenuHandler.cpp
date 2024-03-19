#include "pch.h"

#include "MenuHandler.h"
#include <fstream>
#include <ostream>
#include <engine/Paths.h>
#include <engine/utility/Error.h>
#include <engine/graphics/GraphicsEngine.h>
#include <assets/TextureFactory.h>

#include <nlohmann/json.hpp>
#include "MenuObject.h"

#include "scene/Scene.h"
#include "scene/SceneManager.h"

#include "components/SpriteComponent.h"
#include "components/TextComponent.h"
#include "components/Collider2DComponent.h"
#include "components/CommandComponent.h"

MENU::MenuHandler::MenuHandler()
	: mySceneManager(nullptr)
{}

MENU::MenuHandler::~MenuHandler()
{}

void MENU::MenuHandler::Init(const std::string& aMenuFile, SceneManager* aSceneManager)
{
	//TODO:  ViewPort != RenderSize, add get RenderSize to graphicsengine 23.f is the height of menubar?
	Vector2i viewport = GraphicsEngine::GetInstance()->DX().GetViewportDimensions();
	myRenderSize = { (float)viewport.x, (float)viewport.y + 23.f };
	mySceneManager = aSceneManager;
	LoadFromJson(aMenuFile);
}

void MENU::MenuHandler::Update(const MenuUpdateContext& aContext)
{
	if (myStateStack.empty())
		return;

	for (size_t i = 0; i < myStateStack.top()->objectIds.size(); i++)
	{
		MenuObject& currentObj = myObjectManager.GetObjectFromID(myStateStack.top()->objectIds[i]);
		currentObj.Update(aContext);

		if (!currentObj.HasComponent<CommandComponent>())
			continue;

		if (!currentObj.IsPressed())
			continue;

		auto& command = currentObj.GetComponent<CommandComponent>();
		switch (command.GetCommandType())
		{
		case eCommandType::PopMenu:
		{
			PopState();
			return;
		}
		case eCommandType::PushMenu:
		{
			ID menuID = std::get<ID>(command.GetCommandData().data);
			PushState(menuID);
			return;
		}
		case eCommandType::LoadLevel:
		{
			std::string levelName = std::get<std::string>(command.GetCommandData().data);

			if (!mySceneManager)
			{
				PrintI("Load Level: " + levelName);
				continue;
			}

			mySceneManager->LoadScene({ eSceneType::Game, levelName });
			return;
		}
		case eCommandType::ResumeGame:
		{
			if (!mySceneManager)
			{
				PrintI("Pause/Unpause");
				continue;
			}

			mySceneManager->SetIsPaused(false);
			break;
		}
		case eCommandType::BackToMainMenu:
		{
			if (!mySceneManager)
			{
				PrintI("Back to MainMenu");
				continue;
			}

			while (mySceneManager->GetCurrentScene()->GetType() != eSceneType::MainMenu)
				mySceneManager->PopScene();

			break;
		}
		case eCommandType::QuitGame:
		{
			if (!mySceneManager)
			{
				PrintI("Exit Game!");
				continue;
			}

			mySceneManager->PopScene();
			break;
		}
		default:
			break;
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
	newState.id = myIDManager.GetFreeID();

	myStates.push_back(newState);
	myStateStack.push(&myStates[myStates.size() - 1]);
}

void MENU::MenuHandler::RemoveState(ID aID)
{
	for (size_t i = 0; i < myStates.size(); i++)
	{
		if (myStates[i].id == aID)
		{
			if (GetCurrentState().id == myStates[i].id)
				myStateStack.pop();

			myStates.erase(myStates.begin() + i);
			myIDManager.FreeID(aID);
			return;
		}
	}
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

void MENU::MenuHandler::PopToBaseState()
{
	while (myStateStack.size() > 1)
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
	MenuObject& newObject = myObjectManager.CreateNew(myIDManager.GetFreeID(), aPosition);
	myStateStack.top()->objectIds.push_back(newObject.GetID());

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
	myIDManager.FreeID(aID);
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
	myObjectManager.ClearAll();
	myStates.clear();
	myIDManager.FreeAllIDs();

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
		newState.id = myIDManager.UseID(menuFile["menuState"][i]["ID"]);
		for (size_t j = 0; j < menuFile["menuState"][i]["objects"].size(); j++)
		{
			newState.objectIds.push_back(menuFile["menuState"][i]["objects"][j]);
		}
		myStates.push_back(newState);
	}

	for (size_t i = 0; i < menuFile["objectID"].size(); i++)
	{
		ID id = menuFile["objectID"][i]["objectID"];
		MenuObject& obj = myObjectManager.CreateNew(myIDManager.UseID(id));
		obj.SetName(menuFile["objectID"][i]["name"]);
		obj.SetPosition(JsonToScreenPosition(menuFile["objectID"][i]["position"]));
	}

	nlohmann::json spriteComponents = menuFile["spriteComponents"];
	for (size_t i = 0; i < spriteComponents.size(); i++)
	{
		ID ownerID = spriteComponents[i]["ownerID"];
		SpriteComponent& sprite = myObjectManager.GetObjectFromID(ownerID).AddComponent<SpriteComponent>();

		//TODO: AssetDatabase for sprites?
		nlohmann::json textures = spriteComponents[i]["textures"];
		for (size_t stateIndex = 0; stateIndex < textures.size(); stateIndex++)
		{
			std::string texturePath = textures[stateIndex]["path"];

			if (texturePath == "(None)")
				continue;

			sprite.SetTexture(TextureFactory::CreateTexture(RELATIVE_SPRITE_ASSET_PATH + texturePath, false), texturePath, (ObjectState)stateIndex);
			sprite.SetColor(JsonToColorVec(textures[stateIndex]["color"]), (ObjectState)stateIndex);
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
		ID ownerID = textComponents[i]["ownerID"];

		TextComponent& text = myObjectManager.GetObjectFromID(ownerID).AddComponent<TextComponent>();
		text.SetFont(textComponents[i]["font"]);
		text.SetFontSize((FontSize)textComponents[i]["fontSize"]);
		text.SetText(textComponents[i]["textString"]);
		text.SetPosition(JsonToScreenPosition(textComponents[i]["position"]));

		nlohmann::json colors = textComponents[i]["colors"];
		for (size_t stateIndex = 0; stateIndex < colors.size(); stateIndex++)
		{
			text.SetColor(JsonToColorVec(colors[stateIndex]["color"]), (ObjectState)stateIndex);
		} 

		text.SetIsCentered(textComponents[i]["isCentered"]);

	}

	nlohmann::json colliderComponents = menuFile["colliderComponents"];
	for (size_t i = 0; i < colliderComponents.size(); i++)
	{
		ID ownerID = colliderComponents[i]["ownerID"];
		Collider2DComponent& collider = myObjectManager.GetObjectFromID(ownerID).AddComponent<Collider2DComponent>();

		collider.SetPosition(JsonToScreenPosition(colliderComponents[i]["position"]));
		collider.SetSize(JsonToVec2(colliderComponents[i]["size"]));
	}

	nlohmann::json commandComponents = menuFile["commandComponents"];
	for (size_t i = 0; i < commandComponents.size(); i++)
	{
		ID ownerID = commandComponents[i]["ownerID"];
		CommandComponent& command = myObjectManager.GetObjectFromID(ownerID).AddComponent<CommandComponent>();
		command.SetCommandType((eCommandType)commandComponents[i]["commandType"]);

		if (commandComponents[i].contains("commandData"))
		{
			if (commandComponents[i]["commandData"].is_string())
				command.SetCommandData({ commandComponents[i]["commandData"].get<std::string>() });

			if (commandComponents[i]["commandData"].is_number_integer())
				command.SetCommandData({ commandComponents[i]["commandData"].get<ID>() });
		}
	}

	if (myStates.empty())
	{
		MenuState newState;
		newState.name = "Base";
		newState.id = myIDManager.GetFreeID();
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
				spriteEntry["ownerID"] = sprite.GetParent().GetID();

				for (size_t stateIndex = 0; stateIndex < (int)ObjectState::Count; stateIndex++)
				{
					nlohmann::json textureEntry;
					textureEntry["path"] = sprite.GetTexturePath((ObjectState)stateIndex);
					textureEntry["color"] = ColorVecToJson(sprite.GetColor((ObjectState)stateIndex));

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
			auto texts = myObjectManager.myObjects[i]->GetComponents<TextComponent>();
			for (int componentIndex = 0; componentIndex < texts.size(); componentIndex++)
			{
				TextComponent& text = static_cast<TextComponent&>(*texts[componentIndex]);
				nlohmann::json textEntry;
				textEntry["ownerID"] = text.GetParent().GetID();
				textEntry["textString"] = text.GetText();
				textEntry["position"] = ScreenPositionToJson(text.GetPosition());
				textEntry["font"] = text.GetFontName();
				textEntry["fontSize"] = (int)text.GetFontSize();
				textEntry["isCentered"] = text.GetIsCentered();
				
				for (size_t stateIndex = 0; stateIndex < (int)ObjectState::Count; stateIndex++)
				{
					nlohmann::json colorEntry;
					colorEntry["color"] = ColorVecToJson(text.GetColor((ObjectState)stateIndex));
					textEntry["colors"].push_back(colorEntry);
				}


				textComponents.push_back(textEntry);
			}
		}

		if (myObjectManager.myObjects[i]->HasComponent<Collider2DComponent>())
		{
			Collider2DComponent collider = myObjectManager.myObjects[i]->GetComponent<Collider2DComponent>();
			nlohmann::json colliderEntry;
			colliderEntry["ownerID"] = collider.GetParent().GetID();
			colliderEntry["position"] = ScreenPositionToJson(collider.GetPosition());
			colliderEntry["size"] = Vec2ToJson(collider.GetSize());

			colliderComponents.push_back(colliderEntry);
		}

		if (myObjectManager.myObjects[i]->HasComponent<CommandComponent>())
		{
			CommandComponent command = myObjectManager.myObjects[i]->GetComponent<CommandComponent>();
			nlohmann::json commandEntry;
			commandEntry["ownerID"] = command.GetParent().GetID();
			commandEntry["commandType"] = (int)command.GetCommandType();

			if (std::holds_alternative<std::string>(command.GetCommandData().data))
				commandEntry["commandData"] = std::get<std::string>(command.GetCommandData().data);

			if (std::holds_alternative<ID>(command.GetCommandData().data))
				commandEntry["commandData"] = std::get<ID>(command.GetCommandData().data);

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
