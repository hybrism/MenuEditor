#pragma once
#include "Window.h"

namespace ME
{
	class MenuObject;
	class MenuObjectHierarchy : public ME::WindowBase
	{
	public:
		MenuObjectHierarchy(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags);

		void Show(const UpdateContext& aContext) override;

	private:
		size_t mySelectedIndex;
		void PushMenuObjectToInspector();
	};
}