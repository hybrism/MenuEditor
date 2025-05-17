#pragma once
#include <engine/math/Vector.h>

class SceneManager;
class PostProcess;
class LightManager;

namespace MENU
{
	class MenuHandler;

	struct MenuUpdateContext
	{
		SceneManager* sceneManager = nullptr;
		MenuHandler* menuHandler = nullptr;
		PostProcess* postProcess = nullptr;
		LightManager* lightManager = nullptr;
		float dt;
		Vector2f mousePosition;
		Vector2f mouseDelta;
		Vector2f renderSize;
		bool mouseDown = false;
		bool mouseReleased = false;
	};
}