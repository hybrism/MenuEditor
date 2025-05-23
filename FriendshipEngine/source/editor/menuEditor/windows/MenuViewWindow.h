#pragma once
#include <string>
#include <vector>

#include "Window.h"

class World;
class Texture;

namespace MENU
{
	class MenuViewWindow : public WindowBase
	{
	public:
		MenuViewWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags);


		void Show(const MenuEditorUpdateContext& aContext) override;

	private:

	};
}