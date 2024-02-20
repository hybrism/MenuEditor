#pragma once
#include "Window.h"

namespace ME
{
	class InspectorWindow : public ME::WindowBase
	{
	public:
		InspectorWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags);

		void Show(const UpdateContext& aContext) override;

	private:

	};
}