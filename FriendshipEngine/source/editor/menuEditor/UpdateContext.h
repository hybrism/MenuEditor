#pragma once
#include <vector>
#include <string>
#include <memory>
#include "MenuEditorCommon.h"

class Texture;

namespace MENU
{
	class MenuHandler;
	struct MenuEditorUpdateContext
	{
		Assets assets;
		MenuHandler* menuHandler = nullptr;
		bool showDebugData = false;
	};
}