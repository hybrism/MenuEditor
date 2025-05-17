#include "pch.h"

#include "MenuHandler.h"

#include <engine/graphics/GraphicsEngine.h>
#include <assets/TextureFactory.h>
#include <assets/AssetDatabase.h>

#include <nlohmann/json.hpp>

#include "MenuObject.h"
#include "components/SpriteComponent.h"
#include "components/TextComponent.h"
#include "components/Collider2DComponent.h"
#include "components/CommandComponent.h"
#include "components/InteractableComponent.h"

MENU::MenuHandler::MenuHandler()
{}

MENU::MenuHandler::~MenuHandler()
{}

void MENU::MenuHandler::Init(const std::string& aMenuFile)
{
	//TODO:  ViewPort != RenderSize, add get RenderSize to graphicsengine 23.f is the height of menubar?
	LoadFromJson(aMenuFile);
}

void MENU::MenuHandler::Update(const MenuUpdateContext& aContext)
{
	if (myStateStack.empty())
		return;

	for (size_t i = 0; i < myStateStack.top()->objectIds.size(); i++)
	{
		MenuObject& currentObj = myObjectManager.GetObjectFromID(myStateStack.top()->objectIds[i]);

		if (currentObj.HasComponent<CommandComponent>())
		{
			HandleCommand(currentObj, aContext);
		}

		currentObj.Update(aContext);
	}
}

void MENU::MenuHandler::HandleCommand(MenuObject& aObject, const MenuUpdateContext& aContext)
{
	if (!aObject.IsPressed())
		return;

	if (!aContext.mouseReleased)
		return;

	auto& command = aObject.GetComponent<CommandComponent>();
	command.Execute(command.GetCommandData(), aContext);
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

void MENU::MenuHandler::OnResize(const Vector2i& aNewRenderSize)
{
	myObjectManager.OnResize(aNewRenderSize);
}

MENU::MenuState& MENU::MenuHandler::AddNewState(const std::string& aName)
{
	MenuState newState;

	newState.name = aName;
	newState.id = myIDManager.GetFreeID();

	myStates.push_back(newState);

	return myStates[myStates.size() - 1];
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

void MENU::MenuHandler::PushState(const std::string& aStateName)
{
	for (size_t i = 0; i < myStates.size(); i++)
	{
		if (myStates[i].name == aStateName)
		{
			myStateStack.push(&myStates[i]);
			return;
		}
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

MENU::MenuObject& MENU::MenuHandler::GetObjectFromName(const std::string& aName)
{
	return myObjectManager.GetObjectFromName(aName);
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

MENU::MenuState& MENU::MenuHandler::GetStateFromID(MENU::ID aID)
{
	for (size_t i = 0; i < myStates.size(); i++)
	{
		if (myStates[i].id == aID)
			return myStates[i];
	}

	return *myStateStack.top();
}

std::vector<MENU::MenuState>& MENU::MenuHandler::GetAllStates()
{
	return myStates;
}

std::string MENU::MenuHandler::GetName() const
{
	return myName;
}

std::string MENU::MenuHandler::GetFileName() const
{
	return std::string(myName + ".json");
}

void MENU::MenuHandler::SetName(const std::string& aName)
{
	myName = aName;
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

	myName = aMenuFile.substr(0, aMenuFile.find_last_of('.'));

	std::string MENU_PATH = "menus/";
	std::string path = RELATIVE_IMPORT_DATA_PATH + MENU_PATH + aMenuFile;
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
		obj.SetPosition(JsonToScreen(menuFile["objectID"][i]["position"]), true);
	}

	nlohmann::json spriteComponents = menuFile["spriteComponents"];
	for (size_t i = 0; i < spriteComponents.size(); i++)
	{
		ID ownerID = spriteComponents[i]["ownerID"];
		SpriteComponent& sprite = myObjectManager.GetObjectFromID(ownerID).AddComponent<SpriteComponent>();

		if (spriteComponents[i].contains("name"))
			sprite.myName = spriteComponents[i]["name"];

		nlohmann::json textures = spriteComponents[i]["textures"];
		for (size_t stateIndex = 0; stateIndex < textures.size(); stateIndex++)
		{
			std::string texturePath = textures[stateIndex]["path"];

			if (texturePath == "(None)")
				continue;

			sprite.SetTexture(AssetDatabase::GetTextureDatabase().GetOrLoadSpriteTexture(texturePath), texturePath, (ObjectState)stateIndex);
			sprite.SetColor(JsonToColorVec(textures[stateIndex]["color"]), (ObjectState)stateIndex);
		}

		sprite.SetPosition(JsonToScreen(spriteComponents[i]["position"]));
		sprite.SetSize(JsonToScreen(spriteComponents[i]["size"]));
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

		if (textComponents[i].contains("name"))
			text.myName = textComponents[i]["name"];

		text.SetFont(textComponents[i]["font"]);
		text.SetFontSize((FontSize)textComponents[i]["fontSize"]);
		text.SetText(textComponents[i]["textString"]);
		text.SetPosition(JsonToScreen(textComponents[i]["position"]));

		nlohmann::json colors = textComponents[i]["colors"];
		for (size_t stateIndex = 0; stateIndex < colors.size(); stateIndex++)
		{
			text.SetColor(JsonToColorVec(colors[stateIndex]["color"]), (ObjectState)stateIndex);
		}

		text.SetIsCentered(textComponents[i]["isCentered"]);
	}

	//GraphicsEngine::GetInstance()->GetSpriteRenderer().GetTextService()
	nlohmann::json colliderComponents = menuFile["colliderComponents"];
	for (size_t i = 0; i < colliderComponents.size(); i++)
	{
		ID ownerID = colliderComponents[i]["ownerID"];
		Collider2DComponent& collider = myObjectManager.GetObjectFromID(ownerID).AddComponent<Collider2DComponent>();

		collider.SetPosition(JsonToScreen(colliderComponents[i]["position"]));
		collider.SetSize(JsonToScreen(colliderComponents[i]["size"]));
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

			if (command.GetCommandType() == eCommandType::Resolution)
				if (commandComponents[i]["commandData"].is_number_integer())
					command.SetCommandData({ commandComponents[i]["commandData"].get<int>() });

			if (commandComponents[i]["commandData"].is_number_float())
				command.SetCommandData({ commandComponents[i]["commandData"].get<float>() });
		}
	}

	if (menuFile.contains("interactableComponents"))
	{
		nlohmann::json interactableComponents = menuFile["interactableComponents"];
		for (size_t i = 0; i < interactableComponents.size(); i++)
		{
			ID ownerID = interactableComponents[i]["ownerID"];
			InteractableComponent& interactable = myObjectManager.GetObjectFromID(ownerID).AddComponent<InteractableComponent>();

			for (size_t j = 0; j < interactableComponents[i]["interactions"].size(); j++)
			{
				nlohmann::json interactableComponent = interactableComponents[i]["interactions"][j];

				InteractionType type = (InteractionType)interactableComponent["interactionType"];
				ID parentComponentId = interactableComponent["parentComponent"];

				MenuObject& parent = myObjectManager.GetObjectFromID(ownerID);

				auto sprites = parent.GetComponents<SpriteComponent>();

				for (size_t k = 0; k < sprites.size(); k++)
				{
					if (sprites[k]->GetID() == parentComponentId)
					{
						auto newInteraction = interactable.AddInteraction(sprites[k], type);
						if (newInteraction->myType == InteractionType::Drag)
						{
							auto dragInteraction = std::static_pointer_cast<DragInteraction>(newInteraction);

							dragInteraction->myMin = interactableComponent["min"];
							dragInteraction->myMax = interactableComponent["max"];
						}
					}
				}
			}
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

Vector2f MENU::MenuHandler::JsonToScreen(nlohmann::json aJson)
{
	Vector2f screen = JsonToVec2(aJson);

	Vector2i viewport = GraphicsEngine::GetInstance()->DX().GetViewportDimensions();
	screen.x *= viewport.x;
	screen.y *= viewport.y;

	return screen;
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
		objectData["position"] = ScreenToJson(object.GetPosition());
		objectIds.push_back(objectData);
	}

	menuFile["objectID"] = objectIds;

	nlohmann::json spriteComponents;
	nlohmann::json textComponents;
	nlohmann::json colliderComponents;
	nlohmann::json commandComponents;
	nlohmann::json interactableComponents;

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

				if (!sprite.myName.empty())
					spriteEntry["name"] = sprite.myName;

				for (size_t stateIndex = 0; stateIndex < (int)ObjectState::Count; stateIndex++)
				{
					nlohmann::json textureEntry;
					textureEntry["path"] = sprite.GetTexturePath((ObjectState)stateIndex);
					textureEntry["color"] = ColorVecToJson(sprite.GetColor((ObjectState)stateIndex));

					spriteEntry["textures"].push_back(textureEntry);
				}

				spriteEntry["position"] = ScreenToJson(sprite.GetPosition());
				spriteEntry["size"] = ScreenToJson(sprite.GetSize());
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

				if (!text.myName.empty())
					textEntry["name"] = text.myName;

				textEntry["textString"] = text.GetText();
				textEntry["position"] = ScreenToJson(text.GetPosition());
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
			colliderEntry["position"] = ScreenToJson(collider.GetPosition());
			colliderEntry["size"] = ScreenToJson(collider.GetSize());

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

			if (std::holds_alternative<float>(command.GetCommandData().data))
				commandEntry["commandData"] = std::get<float>(command.GetCommandData().data);

			if (std::holds_alternative<int>(command.GetCommandData().data))
				commandEntry["commandData"] = std::get<int>(command.GetCommandData().data);

			commandComponents.push_back(commandEntry);
		}

		if (myObjectManager.myObjects[i]->HasComponent<InteractableComponent>())
		{
			InteractableComponent interactable = myObjectManager.myObjects[i]->GetComponent<InteractableComponent>();

			nlohmann::json interactableEntry;
			interactableEntry["ownerID"] = interactable.GetParent().GetID();

			nlohmann::json interactions;
			for (size_t interactionIndex = 0; interactionIndex < interactable.myInteractions.size(); interactionIndex++)
			{
				nlohmann::json interactionEntry;

				interactionEntry["interactionType"] = (int)interactable.myInteractions[interactionIndex]->myType;
				interactionEntry["parentComponent"] = (int)interactable.myInteractions[interactionIndex]->myParent->GetID();

				switch (interactable.myInteractions[interactionIndex]->myType)
				{
				case MENU::InteractionType::Drag:
				{
					std::shared_ptr<DragInteraction> drag = std::static_pointer_cast<DragInteraction>(interactable.myInteractions[interactionIndex]);
					interactionEntry["min"] = drag->myMin;
					interactionEntry["max"] = drag->myMax;
					break;
				}
				case MENU::InteractionType::Clip:
				{
					//TODO: Is this necessary?
					break;
				}
				case MENU::InteractionType::Hide:
				{
					//TODO: Is this necessary?
					break;
				}
				default:
					continue;
				}

				interactions.push_back(interactionEntry);
			}

			interactableEntry["interactions"] = interactions;

			interactableComponents.push_back(interactableEntry);
		}
	}

	menuFile["spriteComponents"] = spriteComponents;
	menuFile["textComponents"] = textComponents;
	menuFile["colliderComponents"] = colliderComponents;
	menuFile["commandComponents"] = commandComponents;
	menuFile["interactableComponents"] = interactableComponents;

	std::string MENU_PATH = "menus/";
	std::string path = RELATIVE_IMPORT_DATA_PATH + MENU_PATH + myName + ".json";
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

nlohmann::json MENU::MenuHandler::ScreenToJson(const Vector2f& aPosition)
{
	Vector2i viewport = GraphicsEngine::GetInstance()->DX().GetViewportDimensions();
	Vector2f screenPos = { aPosition.x / viewport.x, aPosition.y / viewport.y };
	return Vec2ToJson(screenPos);
}
