#pragma once
#include <vector>
#include <string>
#include <memory>
#include "MenuEditorCommon.h"

class Texture;

namespace MENU
{
	class MenuHandler;
	struct UpdateContext
	{
		Assets assets;
		MenuHandler* menuHandler = nullptr;
	};
}