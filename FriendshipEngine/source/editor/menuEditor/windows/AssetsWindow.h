#pragma once
#include "Window.h"

namespace MENU
{
	class AssetsWindow : public MENU::WindowBase
	{
	public:
		AssetsWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags);

		void Show(const UpdateContext& aContext) override;

	private:
	};
}