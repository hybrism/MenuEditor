#pragma once
#include <engine/math/Vector.h>

namespace MENU
{
	struct MenuUpdateContext
	{
		Vector2f mousePosition;
		Vector2f renderSize;
		bool mousePressed = false;
	};
}