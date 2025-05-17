#pragma once
#pragma once
#include <string>
#include <vector>

#include "Window.h"

class World;
class Texture;

namespace FE
{
	class GameViewWindow : public WindowBase
	{
	public:
		GameViewWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags);


		void Show(const EditorUpdateContext& aContext) override;

	private:

		int myFPS = 0;
		float myFPSTimer = 0.f;
		float myFPSUpdateFrequency = 1.f;
		bool myShouldPauseFirstFrame = false;

		const char* myGameStates[2] = { "Paused", "Play" };
	};

}