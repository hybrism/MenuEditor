#pragma once
#include <string>
#include <vector>

#include "Window.h"

class World;
class Texture;

namespace ME
{
	class MenuViewWindow : public WindowBase
	{
	public:
		MenuViewWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags);


		void Show(const UpdateContext& aContext) override;

	private:

	};
}