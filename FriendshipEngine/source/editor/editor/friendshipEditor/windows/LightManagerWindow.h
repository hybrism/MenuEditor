#pragma once
#include "Window.h"

namespace FE
{
	class LightManagerWindow : public WindowBase
	{
	public:
		LightManagerWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags);

		void Show(const EditorUpdateContext& aContext) override;

	private:

	};
}