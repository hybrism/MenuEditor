#pragma once
#include "Window.h"

namespace FE
{
	class AssetDatabaseWindow : public WindowBase
	{
	public:
		AssetDatabaseWindow(const std::string& aHandle, bool aOpen, ImGuiWindowFlags aFlags);

		void Show(const EditorUpdateContext& aContext) override;

	private:

	};
}