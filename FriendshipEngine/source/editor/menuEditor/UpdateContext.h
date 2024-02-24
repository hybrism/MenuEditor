#pragma once
#include <vector>
#include <string>
#include <memory>
#include "MenuCommon.h"

class Texture;
class MenuHandler;

namespace MENU
{
	struct UpdateContext
	{
		Assets assets;
		MenuHandler* menuHandler = nullptr;
	};
}