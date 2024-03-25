#pragma once
#include <engine/math/Vector.h>

namespace MENU
{
	struct MenuUpdateContext
	{
		float dt;
		Vector2f mousePosition;
		Vector2f mouseDelta;
		Vector2f renderSize;
		bool mousePressed = false;
		bool mouseReleased = false;
	};
}