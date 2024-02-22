#pragma once
#include "Window.h"

namespace ME
{
	class AssetsWindow : public ME::WindowBase
	{
	public:
		AssetsWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags);

		void Show(const UpdateContext& aContext) override;

	private:
	};
}