#include "MenuHandler.h"
#include <nlohmann/json.hpp>

#include "menuObject/MenuObject.h"

#include "components/SpriteComponent.h"
#include "components/TextComponent.h"
#include "components/Collider2DComponent.h"

using namespace ME;

MenuHandler::MenuHandler()
{
}

MenuHandler::~MenuHandler()
{
}

void MenuHandler::Init(const std::string& aMenuFile)
{
	LoadFromJson(aMenuFile);
}

void MenuHandler::Update()
{
	myObjectManager.Update();
}

void MenuHandler::Render()
{
	myObjectManager.Render();
}

void MenuHandler::LoadFromJson(const std::string& aMenuFile)
{
	myName = aMenuFile;

	aMenuFile;
}

void MenuHandler::SaveToJson()
{
}
